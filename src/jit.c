#define _DEFAULT_SOURCE
#include "jit.h"
#include "assert.h"
#include "log.h"
#include "stdjit.h"
#include <sys/mman.h>
#include "ass.h"

#define PAGE_SIZE (4096)

typedef int (*jited_function)(void);

struct libjit_unit {
	struct libjit_ast *ast;
	bool jited;
	void *map;
	size_t page_count;
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
	unit->map = NULL;
	unit->page_count = 0;
}

static void compile_node(struct libjit_ast *ast, void *user_data)
{
	uint8_t **data = user_data;
	switch (ast->op) {
	case ADD:
		*data += write_instr(INSTR_POP_B, 0, *data);
		*data += write_instr(INSTR_POP_A, 0, *data);
		*data += write_instr(INSTR_ADD, 0, *data);
		*data += write_instr(INSTR_PUSH_A, 0, *data);
		break;
	case SUB:
		*data += write_instr(INSTR_POP_B, 0, *data);
		*data += write_instr(INSTR_POP_A, 0, *data);
		*data += write_instr(INSTR_SUB, 0, *data);
		*data += write_instr(INSTR_PUSH_A, 0, *data);
		break;
	case MULT:
		*data += write_instr(INSTR_POP_B, 0, *data);
		*data += write_instr(INSTR_POP_A, 0, *data);
		*data += write_instr(INSTR_MULT, 0, *data);
		*data += write_instr(INSTR_PUSH_A, 0, *data);
		break;
	case DIV:
		*data += write_instr(INSTR_POP_B, 0, *data);
		*data += write_instr(INSTR_POP_A, 0, *data);
		*data += write_instr(INSTR_DIV, 0, *data);
		*data += write_instr(INSTR_PUSH_A, 0, *data);
		break;
	case ATOM:
		*data += write_instr(INSTR_PUSH_IMM, ast->value, *data);
		break;
	}
}

static bool jit_unit(struct libjit_unit *unit)
{
	uint8_t *curr = unit->map;

	libjit_postorder(unit->ast, compile_node, &curr);

	curr += write_instr(INSTR_POP_A, 0, curr);
	curr += write_instr(INSTR_RET, 0, curr);

	mprotect(unit->map, unit->page_count * PAGE_SIZE,
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

int libjit_ctx_evaluate(struct libjit_ctx *ctx, libjit_handle hdl)
{
	ASSERT(ctx != NULL, "NULL context");
	ASSERT(hdl < ctx->ast_num, "AST doesn't exist");
	if (!ctx->units[hdl].jited)
		return libjit_evaluate(ctx->units[hdl].ast);

	jited_function f = (jited_function)ctx->units[hdl].map;
	return f();
}

bool libjit_ctx_jit(struct libjit_ctx *ctx, libjit_handle hdl)
{
	ASSERT(ctx != NULL, "NULL context");
	ASSERT(hdl < ctx->ast_num, "AST doesn't exist");

	if (ctx->units[hdl].jited)
		return true;

	ctx->units[hdl].jited = false;
	ctx->units[hdl].map = map(1);
	ctx->units[hdl].page_count = 1;

	ctx->units[hdl].jited = jit_unit(ctx->units + hdl);
	return ctx->units[hdl].jited;
}
