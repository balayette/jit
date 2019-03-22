#ifndef AST_H
#define AST_H

#include <stddef.h>

enum libjit_op { ADD, SUB, MULT, DIV, ATOM, CALL };

struct libjit_ast {
	union {
		int value;
		size_t hdl;
	};
	enum libjit_op op;
	struct libjit_ast *left;
	struct libjit_ast *right;
};

void libjit_preorder(struct libjit_ast *ast,
		     void (*f)(struct libjit_ast *, void *), void *data);

void libjit_inorder(struct libjit_ast *ast,
		    void (*f)(struct libjit_ast *, void *), void *data);

void libjit_postorder(struct libjit_ast *ast,
		      void (*f)(struct libjit_ast *, void *), void *data);

void libjit_free_ast(struct libjit_ast *ast);

struct libjit_ast *libjit_create_ast(enum libjit_op op, struct libjit_ast *left,
				     struct libjit_ast *right);

int libjit_evaluate(struct libjit_ast *ast);

#endif /* AST_H */
