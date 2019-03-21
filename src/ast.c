#include "ast.h"
#include "stdjit.h"
#include "log.h"

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

int libjit_evaluate(struct libjit_ast *ast)
{
	switch(ast->op)
	{
		case ATOM:
			return ast->value;
		case ADD:
			return libjit_evaluate(ast->left) + libjit_evaluate(ast->right);
		case SUB:
			return libjit_evaluate(ast->left) - libjit_evaluate(ast->right);
		case MULT:
			return libjit_evaluate(ast->left) * libjit_evaluate(ast->right);
		case DIV:
			return libjit_evaluate(ast->left) / libjit_evaluate(ast->right);
	}

	LIBJIT_DIE("unreachable");
	return 0;
}
