#include "ast.h"
#include "stdjit.h"

void libjit_free_ast(struct libjit_ast *ast)
{
	if (!ast)
		return;

	libjit_free_ast(ast->left);
	libjit_free_ast(ast->right);

	libjit_free(ast);
}

struct libjit_ast *libjit_create_ast(enum libjit_op op, struct libjit_ast *left, struct libjit_ast *right)
{
	struct libjit_ast *ast = libjit_malloc(sizeof(struct libjit_ast));
	ast->op = op;
	ast->left = left;
	ast->right = right;

	return ast;
}
