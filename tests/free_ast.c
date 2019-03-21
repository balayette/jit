#include "test.h"
#include "ast.h"
#include <stdlib.h>

int main(void)
{
	struct libjit_ast *left = libjit_create_ast(ATOM, NULL, NULL);
	struct libjit_ast *right = libjit_create_ast(ATOM, NULL, NULL);
	struct libjit_ast *root = libjit_create_ast(ADD, left, right);

	libjit_free_ast(root);

	return TEST_SUCESS;
}
