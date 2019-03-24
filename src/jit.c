#define _DEFAULT_SOURCE
#include "jit.h"
#include "assert.h"
#include "log.h"
#include "stdjit.h"
#include <sys/mman.h>
#include "ass.h"

#define PAGE_SIZE (4096)

typedef libjit_value (*jited_function)(void);

struct libjit_execution_unit {
	struct libjit_ctx *context;
	libjit_handle hdl;
	uint8_t code[];
};

struct libjit_unit {
	struct libjit_ast *ast;
	size_t page_count;
	struct libjit_execution_unit *exec_unit;
	bool jited;
};

static void *map(size_t page_count)
{
	void *ret = mmap(NULL, page_count * PAGE_SIZE, PROT_WRITE,
			 MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	ASSERT(ret != MAP_FAILED, "Failed to map %zu pages.", page_count);

	return ret;
}

static void setup_unit(struct libjit_unit *unit, struct libjit_ast *ast)
{
	unit->ast = ast;
	unit->jited = false;
	unit->exec_unit = NULL;
	unit->page_count = 0;
}

struct compile_data {
	uint8_t *curr;
	struct libjit_ctx *ctx;
	struct libjit_unit *unit;
};

static libjit_value libjit_evaluate(struct libjit_ctx *ctx, struct libjit_ast *ast)
{
	switch (ast->op) {
	case ATOM:
		return ast->value;
	case ADD:
		return libjit_evaluate(ctx, ast->left) +
		       libjit_evaluate(ctx, ast->right);
	case SUB:
		return libjit_evaluate(ctx, ast->left) -
		       libjit_evaluate(ctx, ast->right);
	case MULT:
		return libjit_evaluate(ctx, ast->left) *
		       libjit_evaluate(ctx, ast->right);
	case DIV:
		return libjit_evaluate(ctx, ast->left) /
		       libjit_evaluate(ctx, ast->right);
	case CALL:
		return libjit_ctx_evaluate(ctx, ast->hdl);
	}

	LIBJIT_DIE("unreachable");
	return 0;
}

#define OPER(instr, value, addr, curr)                                        \
	curr += write_operation(instr, value, addr, curr)

static void compile_node(struct libjit_ast *ast, void *user_data)
{
	struct compile_data *cdata = user_data;
	switch (ast->op) {
	case ADD:
		OPER(OPER_POP_B, 0, 0, cdata->curr);
		OPER(OPER_POP_A, 0, 0, cdata->curr);
		OPER(OPER_ADD, 0, 0, cdata->curr);
		OPER(OPER_PUSH_A, 0, 0, cdata->curr);
		break;
	case SUB:
		OPER(OPER_POP_B, 0, 0, cdata->curr);
		OPER(OPER_POP_A, 0, 0, cdata->curr);
		OPER(OPER_SUB, 0, 0, cdata->curr);
		OPER(OPER_PUSH_A, 0, 0, cdata->curr);
		break;
	case MULT:
		OPER(OPER_POP_B, 0, 0, cdata->curr);
		OPER(OPER_POP_A, 0, 0, cdata->curr);
		OPER(OPER_MULT, 0, 0, cdata->curr);
		OPER(OPER_PUSH_A, 0, 0, cdata->curr);
		break;
	case DIV:
		OPER(OPER_POP_B, 0, 0, cdata->curr);
		OPER(OPER_POP_A, 0, 0, cdata->curr);
		OPER(OPER_DIV, 0, 0, cdata->curr);
		OPER(OPER_PUSH_A, 0, 0, cdata->curr);
		break;
	case CALL:
		OPER(OPER_PUSH_IMM, ast->hdl, 0, cdata->curr);
		OPER(OPER_PUSH_ADDR, 0, (size_t)cdata->ctx, cdata->curr);
		OPER(OPER_POP_PARAM1, 0, 0, cdata->curr);
		OPER(OPER_POP_PARAM2, 0, 0, cdata->curr);
		OPER(OPER_CALL, 0, (size_t)&libjit_ctx_evaluate, cdata->curr);
		OPER(OPER_PUSH_A, 0, 0, cdata->curr);
		break;
	case ATOM:
		OPER(OPER_PUSH_IMM, ast->value, 0, cdata->curr);
		break;
	}
}

static bool jit_unit(struct libjit_ctx *ctx, struct libjit_unit *unit)
{
	struct compile_data data = { .curr = unit->exec_unit->code,
				     .ctx = ctx,
				     .unit = unit };

	libjit_postorder(unit->ast, compile_node, &data);

	data.curr += write_operation(OPER_POP_A, 0, 0, data.curr);
	data.curr += write_operation(OPER_RET, 0, 0, data.curr);

	mprotect(unit->exec_unit, unit->page_count * PAGE_SIZE,
		 PROT_READ | PROT_EXEC);

	return true;
}

struct libjit_ctx {
	struct libjit_unit *units;
	size_t ast_num;
	size_t ast_max;
};

struct libjit_ctx *libjit_create_ctx(size_t asts)
{
	struct libjit_ctx *ctx = libjit_malloc(sizeof(struct libjit_ctx));
	ctx->units = libjit_calloc(asts, sizeof(struct libjit_unit));

	ctx->ast_num = 0;
	ctx->ast_max = asts;

	return ctx;
}

void libjit_free_ctx(struct libjit_ctx *ctx, bool free_asts)
{
	ASSERT(ctx != NULL, "NULL context");

	if (free_asts) {
		for (size_t i = 0; i < ctx->ast_num; i++)
			libjit_free_ast(ctx->units[i].ast);
	}

	for (size_t i = 0; i < ctx->ast_num; i++) {
		if (ctx->units[i].jited)
			munmap(ctx->units[i].exec_unit,
			       PAGE_SIZE * ctx->units[i].page_count);
	}

	libjit_free(ctx->units);
	libjit_free(ctx);
}

libjit_handle libjit_ctx_add_ast(struct libjit_ctx *ctx, struct libjit_ast *ast)
{
	ASSERT(ctx != NULL, "NULL context");
	ASSERT(ast != NULL, "NULL ast");

	if (ctx->ast_max == ctx->ast_num) {
		ctx->ast_max *= 2;
		ctx->units = libjit_reallocarray(ctx->units, ctx->ast_max,
						 sizeof(struct libjit_unit));
	}

	setup_unit(ctx->units + ctx->ast_num, ast);
	return ctx->ast_num++;
}

libjit_value libjit_ctx_evaluate(struct libjit_ctx *ctx, libjit_handle hdl)
{
	ASSERT(ctx != NULL, "NULL context");
	ASSERT(hdl < ctx->ast_num, "AST doesn't exist");
	if (!ctx->units[hdl].jited)
		return libjit_evaluate(ctx, ctx->units[hdl].ast);

	jited_function f = (jited_function)ctx->units[hdl].exec_unit->code;
	return f();
}

bool libjit_ctx_jit(struct libjit_ctx *ctx, libjit_handle hdl)
{
	ASSERT(ctx != NULL, "NULL context");
	ASSERT(hdl < ctx->ast_num, "AST doesn't exist");

	if (ctx->units[hdl].jited)
		return true;

	ctx->units[hdl].jited = false;
	ctx->units[hdl].exec_unit = map(1);
	ctx->units[hdl].page_count = 1;

	ctx->units[hdl].jited = jit_unit(ctx, ctx->units + hdl);
	return ctx->units[hdl].jited;
}
