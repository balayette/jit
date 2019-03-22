#include "ass.h"
#include "log.h"

#define ADD_RBX_RAX (0x00d80148)
#define SUB_RBX_RAX (0x00d82948)
#define MUL_RBX (0x00ebf748)
#define CLEAR_RDX (0x99)
#define DIV_RBX (0x00fbf748)
#define PUSH_SMALL (0x6a)
#define PUSH_LARGE (0x68)
#define POP_RAX (0x58)
#define POP_RBX (0x5b)
#define PUSH_RAX (0x50)
#define PUSH_RBX (0x53)
#define RET (0xc3)

size_t write_instr(enum instr instr, int value, uint8_t *offset)
{
	switch (instr) {
	case INSTR_ADD:
		*((uint32_t *)offset) = ADD_RBX_RAX;
		return 3;
	case INSTR_SUB:
		*((uint32_t *)offset) = SUB_RBX_RAX;
		return 3;
	case INSTR_MULT:
		*((uint32_t *)offset) = MUL_RBX;
		return 3;
	case INSTR_DIV:
		*offset = CLEAR_RDX;
		offset++;
		*((uint32_t *)offset) = DIV_RBX;
		return 4;
	case INSTR_RET:
		*((uint8_t *)offset) = RET;
		return 1;
	case INSTR_PUSH_IMM:
		*offset = PUSH_LARGE;
		*((uint32_t *)(offset + 1)) = value;
		return 5;
	case INSTR_PUSH_A:
		*offset = PUSH_RAX;
		return 1;
	case INSTR_PUSH_B:
		*offset = PUSH_RBX;
		return 1;
	case INSTR_POP_A:
		*offset = POP_RAX;
		return 1;
	case INSTR_POP_B:
		*offset = POP_RBX;
		return 1;
	default:
		LIBJIT_DIE("Unhandled instruction\n");
	}

	return 0;
}
