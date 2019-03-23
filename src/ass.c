#include "ass.h"
#include "log.h"
#include "jit.h"

struct instruction {
	uint64_t opcode;
	const char *str;
	size_t opcode_size;
	bool payload_address;
	bool payload_value;
	size_t payload_size;
};

enum instruction_e {
	ADD_RBX_RAX = 0,
	SUB_RBX_RAX,
	MUL_RBX,
	CLEAR_RDX,
	DIV_RBX,
	PUSH_SMALL,
	PUSH_LARGE,
	POP_RAX,
	POP_RBX,
	POP_RDI,
	POP_RSI,
	PUSH_RAX,
	PUSH_RBX,
	PUSH_RCX,
	MOV_RAX_RDI,
	MOV_IMM_RAX,
	MOV_IMM_RCX_LARGE,
	CALL_RCX,
	RET,
};

struct instruction instructions[] = {
	[ADD_RBX_RAX] = {
		.opcode = 0x00d80148,
		.str = "ADD_RBX_RAX",
		.opcode_size = 3,
		.payload_address = false,
		.payload_value = false,
	},
	[SUB_RBX_RAX] = {
		.opcode = 0x00d82948,
		.str = "SUB_RBX_RAX",
		.opcode_size = 3,
		.payload_address = false,
		.payload_value = false,
	},
	[MUL_RBX] = {
		.opcode = 0x00ebf748,
		.str = "MUL_RBX",
		.opcode_size = 3,
		.payload_address = false,
		.payload_value = false,
	},
	[CLEAR_RDX] = {
		.opcode = 0x99,
		.str = "CLEAR_RDX",
		.opcode_size = 1,
		.payload_address = false,
		.payload_value = false,
	},
	[DIV_RBX] = {
		.opcode = 0x00fbf748,
		.str = "DIV_RBX",
		.opcode_size = 3,
		.payload_address = false,
		.payload_value = false,
	},
	[PUSH_SMALL] = {
		.opcode = 0x6a,
		.str = "PUSH_SMALL",
		.opcode_size = 1,
		.payload_address = false,
		.payload_value = false,
	},
	[PUSH_LARGE] = {
		.opcode = 0x68,
		.str = "PUSH_LARGE",
		.opcode_size = 1,
		.payload_address = false,
		.payload_value = false,
	},
	[POP_RAX] = {
		.opcode = 0x58,
		.str = "POP_RAX",
		.opcode_size = 1,
		.payload_address = false,
		.payload_value = false,
	},
	[POP_RBX] = {
		.opcode = 0x5b,
		.str = "POP_RBX",
		.opcode_size = 1,
		.payload_address = false,
		.payload_value = false,
	},
	[POP_RDI] = {
		.opcode = 0x5f,
		.str = "POP_RDI",
		.opcode_size = 1,
		.payload_address = false,
		.payload_value = false,
	},
	[POP_RSI] = {
		.opcode = 0x5e,
		.str = "POP_RSI",
		.opcode_size = 1,
		.payload_address = false,
		.payload_value = false,
	},
	[PUSH_RAX] = {
		.opcode = 0x50,
		.str = "PUSH_RAX",
		.opcode_size = 1,
		.payload_address = false,
		.payload_value = false,
	},
	[PUSH_RBX] = {
		.opcode = 0x53,
		.str = "PUSH_RBX",
		.opcode_size = 1,
		.payload_address = false,
		.payload_value = false,
	},
	[PUSH_RCX] = {
		.opcode = 0x51,
		.str = "PUSH_RCX",
		.opcode_size = 1,
		.payload_address = false,
		.payload_value = false,
	},
	[MOV_RAX_RDI] = {
		.opcode = 0x00c78948,
		.str = "MOV_RAX_RDI",
		.opcode_size = 3,
		.payload_address = false,
		.payload_value = false,
	},
	[MOV_IMM_RAX] = {
		.opcode = 0x00c0c748,
		.str = "MOV_IMM_RAX",
		.opcode_size = 3,
		.payload_address = false,
		.payload_value = true,
		.payload_size = 4,
	},
	[MOV_IMM_RCX_LARGE] = {
		.opcode = 0xb948,
		.str = "MOV_IMM_RCX_LARGE",
		.opcode_size = 2,
		.payload_address = true,
		.payload_value = false,
		.payload_size = 8,
	},
	[CALL_RCX] = {
		.opcode = 0xd1ff,
		.str = "CALL_RCX",
		.opcode_size = 2,
		.payload_address = false,
		.payload_value = false,
	},
	[RET] = {
		.opcode = 0xc3,
		.str = "RET",
		.opcode_size = 1,
		.payload_address = false,
		.payload_value = false,
	},
};

#define ADD_RBX_RAX (0x00d80148)
#define SUB_RBX_RAX (0x00d82948)
#define MUL_RBX (0x00ebf748)
#define CLEAR_RDX (0x99)
#define DIV_RBX (0x00fbf748)
#define PUSH_SMALL (0x6a)
#define PUSH_LARGE (0x68)
#define POP_RAX (0x58)
#define POP_RBX (0x5b)
#define POP_RDI (0x5f)
#define POP_RSI (0x5e)
#define PUSH_RAX (0x50)
#define PUSH_RBX (0x53)
#define PUSH_RCX (0x51)
#define MOV_RAX_RDI (0x00c78948)
#define MOV_IMM_RAX (0x00c0c748)
#define MOV_IMM_RCX_LARGE (0xb948)
#define CALL_RCX (0xd1ff)
#define RET (0xc3)

size_t write_instr(enum instr instr, libjit_value value, size_t addr, uint8_t *offset)
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
	case INSTR_PUSH_ADDR:
		*(uint16_t *)offset = MOV_IMM_RCX_LARGE;
		offset += 2;
		*(uint64_t *)offset = addr;
		offset += 8;
		*offset = PUSH_RCX;
		return 11;
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
	case INSTR_POP_PARAM1:
		*offset = POP_RDI;
		return 1;
	case INSTR_POP_PARAM2:
		*offset = POP_RSI;
		return 1;
	case INSTR_CALL:
		*(uint16_t *)offset = MOV_IMM_RCX_LARGE;
		offset += 2;
		*(uint64_t *)offset = addr;
		offset += 8;
		*(uint16_t *)offset = CALL_RCX;
		return 12;
	default:
		LIBJIT_DIE("Unhandled instruction\n");
	}

	return 0;
}
