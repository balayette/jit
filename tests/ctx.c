#include "ast.h"
#include "jit.h"
#include <stdio.h>
#include <criterion/criterion.h>

Test(ctx_evaluate, passing)
{
	struct libjit_ast *n8989 = libjit_create_ast(ATOM, NULL, NULL);
	n8989->value = 8989;

	struct libjit_ast *n4 = libjit_create_ast(ATOM, NULL, NULL);
	n4->value = 4;

	struct libjit_ast *n324 = libjit_create_ast(ATOM, NULL, NULL);
	n324->value = 324;

	struct libjit_ast *n223 = libjit_create_ast(ATOM, NULL, NULL);
	n223->value = 223;

	struct libjit_ast *n67 = libjit_create_ast(ATOM, NULL, NULL);
	n67->value = 67;

	struct libjit_ast *div = libjit_create_ast(DIV, n8989, n4);
	struct libjit_ast *sub = libjit_create_ast(SUB, div, n67);

	struct libjit_ast *mul = libjit_create_ast(MULT, n324, n223);

	struct libjit_ast *add = libjit_create_ast(ADD, sub, mul);

	struct libjit_ctx *ctx = libjit_create_ctx(1);
	libjit_handle hdl = libjit_ctx_add_ast(ctx, add);

	int ret = libjit_ctx_evaluate(ctx, hdl);

	cr_assert(ret == 74432);

	libjit_free_ctx(ctx, true);
}

Test(ctx_jit, passing)
{
	struct libjit_ast *n8989 = libjit_create_ast(ATOM, NULL, NULL);
	n8989->value = 8989;

	struct libjit_ast *n4 = libjit_create_ast(ATOM, NULL, NULL);
	n4->value = 4;

	struct libjit_ast *n324 = libjit_create_ast(ATOM, NULL, NULL);
	n324->value = 324;

	struct libjit_ast *n223 = libjit_create_ast(ATOM, NULL, NULL);
	n223->value = 223;

	struct libjit_ast *n67 = libjit_create_ast(ATOM, NULL, NULL);
	n67->value = 67;

	struct libjit_ast *div = libjit_create_ast(DIV, n8989, n4);
	struct libjit_ast *sub = libjit_create_ast(SUB, div, n67);

	struct libjit_ast *mul = libjit_create_ast(MULT, n324, n223);

	struct libjit_ast *add = libjit_create_ast(ADD, sub, mul);

	struct libjit_ctx *ctx = libjit_create_ctx(1);
	libjit_handle hdl = libjit_ctx_add_ast(ctx, add);

	cr_assert(libjit_ctx_jit(ctx, hdl));
	cr_assert(libjit_ctx_evaluate(ctx, hdl) == 74432);
}
