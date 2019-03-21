#ifndef AST_H
#define AST_H

enum libjit_op { ADD,
		 SUB,
		 MULT,
		 DIV,
		 ATOM };

struct libjit_ast {
	int value;
	enum libjit_op op;
	struct libjit_ast *left;
	struct libjit_ast *right;
};

void libjit_free_ast(struct libjit_ast *ast);

struct libjit_ast *libjit_create_ast(enum libjit_op op, struct libjit_ast *left, struct libjit_ast *right);

int libjit_evaluate(struct libjit_ast *ast);

#endif /* AST_H */
