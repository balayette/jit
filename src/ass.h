#ifndef ASS_H
#define ASS_H

#include <stddef.h>
#include <stdint.h>
#include "ast.h"

/* I'm going to be using stack based evaluation because it's easier this way */

#define SIMPLE_OPER_BEGIN (OPER_ADD)
#define SIMPLE_OPER_END (OPER_RET)

#define COMPLEX_OPER_BEGIN (OPER_PUSH_IMM)
#define COMPLEX_OPER_END (OPER_CALL)

enum operation {
	/* Simple param-less operations. */
	/* Math ops */
	OPER_ADD = 0,
	OPER_SUB,
	OPER_MULT,
	OPER_DIV,
	/* Store / load */
	OPER_PUSH_A,
	OPER_PUSH_B,
	OPER_POP_A,
	OPER_POP_B,
	OPER_POP_PARAM1,
	OPER_POP_PARAM2,
	/* Misc */
	OPER_RET,

	/* Complex operations*/
	OPER_PUSH_IMM,
	OPER_PUSH_ADDR,
	OPER_CALL,

	/* Number of operations*/
	OPER_COUNT,
};

size_t write_operation(enum operation operation, libjit_value value, size_t addr,
		   uint8_t *offset);

#endif /* !ASS_H */
