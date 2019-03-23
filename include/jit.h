#ifndef JIT_H
#define JIT_H

#include "ast.h"
#include <stddef.h>
#include <stdbool.h>

typedef size_t libjit_handle;

struct libjit_ctx;

libjit_handle libjit_ctx_add_ast(struct libjit_ctx *ctx,
				 struct libjit_ast *ast);

struct libjit_ctx *libjit_create_ctx(size_t asts);

void libjit_free_ctx(struct libjit_ctx *ctx, bool free_asts);

libjit_value libjit_ctx_evaluate(struct libjit_ctx *ctx, libjit_handle hdl);

bool libjit_ctx_jit(struct libjit_ctx *ctx, libjit_handle hdl);

#endif /* !JIT_H */
