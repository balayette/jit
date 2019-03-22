#ifndef ASS_H
#define ASS_H

#include <stddef.h>
#include <stdint.h>

/* I'm going to be using stack based evaluation because it's easier this way */

enum instr {
	INSTR_ADD,
	INSTR_SUB,
	INSTR_MULT,
	INSTR_DIV,
	INSTR_PUSH_A,
	INSTR_PUSH_B,
	INSTR_POP_A,
	INSTR_POP_B,
	INSTR_PUSH_IMM,
	INSTR_PUSH_ADDR,
	INSTR_POP_PARAM1,
	INSTR_POP_PARAM2,
	INSTR_CALL,
	INSTR_RET
};

size_t write_instr(enum instr instr, int value, size_t addr, uint8_t *offset);

#endif /* !ASS_H */
