#include "ast.h"
#include <stddef.h>
#include <stdio.h>
#include <criterion/criterion.h>

Test(evaluate, passing) {
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

	int ret = libjit_evaluate(n8989);
	cr_assert(ret == 8989);

	ret = libjit_evaluate(div);
	cr_assert(ret == 8989 / 4);

	ret = libjit_evaluate(mul);
	cr_assert(ret == 324 * 223);

	ret = libjit_evaluate(sub);
	cr_assert(ret ==  (8989 / 4) - 67);

	ret = libjit_evaluate(add);

	cr_assert(ret == 74432);

	libjit_free_ast(add);
}
