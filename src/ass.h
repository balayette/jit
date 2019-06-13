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
	/* Store / load */
	OPER_PUSH_A,
	OPER_PUSH_B,
	OPER_POP_A,
	OPER_POP_B,
	OPER_POP_PARAM1,
	OPER_POP_PARAM2,
	/* Control flow */
	OPER_FUNCTION_EPILOGUE,
	OPER_RET,

	/* Complex operations*/
	/* Math ops */
	OPER_DIV,
	OPER_MOD,
	/* Store / load */
	OPER_PUSH_IMM,
	/* Control flow */
	OPER_FUNCTION_PROLOGUE,
	OPER_CALL,

	/* Number of operations*/
	OPER_COUNT,
};

uint8_t *write_operation(enum operation operation, size_t value,
			 uint8_t *offset);

void dump_instructions(uint8_t *offset, uint8_t *end_offset);

#endif /* !ASS_H */
