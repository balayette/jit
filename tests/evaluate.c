#include "ast.h"
#include "jit.h"
#include <stddef.h>
#include <stdio.h>
#include <criterion/criterion.h>

Test(evaluate, passing)
{
	// +
	//   -
	//     /
	//       8989
	//       4
	//     67
	//   *
	//     324
	//     223
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

	struct libjit_ast *mod = libjit_create_ast(MOD, add, n67);

	struct libjit_ctx *ctx = libjit_create_ctx(10);

	libjit_handle h1 = libjit_ctx_add_ast(ctx, n8989);
	libjit_handle h2 = libjit_ctx_add_ast(ctx, n4);
	libjit_handle h3 = libjit_ctx_add_ast(ctx, n324);
	libjit_handle h4 = libjit_ctx_add_ast(ctx, n223);
	libjit_handle h5 = libjit_ctx_add_ast(ctx, n67);
	libjit_handle h6 = libjit_ctx_add_ast(ctx, div);
	libjit_handle h7 = libjit_ctx_add_ast(ctx, sub);
	libjit_handle h8 = libjit_ctx_add_ast(ctx, mul);
	libjit_handle h9 = libjit_ctx_add_ast(ctx, add);
	libjit_handle h10 = libjit_ctx_add_ast(ctx, mod);

	cr_assert(libjit_ctx_evaluate(ctx, h1) == 8989);
	cr_assert(libjit_ctx_evaluate(ctx, h2) == 4);
	cr_assert(libjit_ctx_evaluate(ctx, h3) == 324);
	cr_assert(libjit_ctx_evaluate(ctx, h4) == 223);
	cr_assert(libjit_ctx_evaluate(ctx, h5) == 67);
	cr_assert(libjit_ctx_evaluate(ctx, h6) == 8989 / 4);
	cr_assert(libjit_ctx_evaluate(ctx, h7) == (8989 / 4) - 67);
	cr_assert(libjit_ctx_evaluate(ctx, h8) == 324 * 223);
	cr_assert(libjit_ctx_evaluate(ctx, h9) == 74432);
	cr_assert(libjit_ctx_evaluate(ctx, h10) == 74432 % 67);

	libjit_free_ctx(ctx, false);
	libjit_free_ast(add);
}
