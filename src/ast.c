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

struct libjit_ast *libjit_create_ast(enum libjit_op op, struct libjit_ast *left,
				     struct libjit_ast *right)
{
	struct libjit_ast *ast = libjit_malloc(sizeof(struct libjit_ast));
	ast->op = op;
	ast->left = left;
	ast->right = right;

	return ast;
}

void libjit_postorder(struct libjit_ast *ast,
		      void (*f)(struct libjit_ast *, void *), void *data)
{
	if (!ast)
		return;

	libjit_postorder(ast->left, f, data);
	libjit_postorder(ast->right, f, data);

	f(ast, data);
}

void libjit_inorder(struct libjit_ast *ast,
		    void (*f)(struct libjit_ast *, void *), void *data)
{
	if (!ast)
		return;

	libjit_inorder(ast->left, f, data);

	f(ast, data);

	libjit_inorder(ast->right, f, data);
}

void libjit_preorder(struct libjit_ast *ast,
		     void (*f)(struct libjit_ast *, void *), void *data)
{
	if (!ast)
		return;

	f(ast, data);

	libjit_preorder(ast->left, f, data);
	libjit_preorder(ast->right, f, data);
}
