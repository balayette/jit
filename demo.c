#include "jit.h"
#include <time.h>
#include <stdio.h>

int main(void)
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

	clock_t t1 = clock();
	for (int i = 0; i < 1000000; i++)
		libjit_ctx_evaluate(ctx, hdl);
	t1 = clock() - t1;
	printf("Time taken (NON JIT): %f\n", (double)t1 / CLOCKS_PER_SEC);

	libjit_ctx_jit(ctx, hdl);
	clock_t t2 = clock();

	for (int i = 0; i < 1000000; i++)
		libjit_ctx_evaluate(ctx, hdl);
	t2 = clock() - t2;

	printf("Time taken (JIT): %f\n", (double)t2 / CLOCKS_PER_SEC);

	printf("%ld%% faster.\n", 100 - t2 * 100 / t1);

	libjit_free_ctx(ctx, 1);

	return 0;
}
