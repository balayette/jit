#include "ass.h"
#include "assert.h"
#include "log.h"
#include "jit.h"
#include <string.h>

/* Only used for readability. */
#define NO_VALUE (-1)

struct instruction {
	uint8_t opcode[15];
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
	POP_RAX,
	POP_RBX,
	POP_RDI,
	POP_RSI,
	PUSH_RAX,
	PUSH_RBX,
	PUSH_RCX,
	MOV_RAX_RDI,
	MOV_RDX_RAX,
	MOV_IMM_RAX,
	MOV_IMM_RCX_LARGE,
	CALL_RCX,
	PUSH_RBP,
	MOV_RSP_RBP,
	LEAVE,
	RET,
	INSTR_COUNT,
};

static struct instruction instructions[] = {
	[ADD_RBX_RAX] = {
		.opcode = {0x48, 0x01, 0xd8},
		.str = "add %rbx, %rax",
		.opcode_size = 3,
		.payload_address = false,
		.payload_value = false,
	},
	[SUB_RBX_RAX] = {
		.opcode = {0x48, 0x29, 0xd8},
		.str = "sub %rbx, %rax",
		.opcode_size = 3,
		.payload_address = false,
		.payload_value = false,
	},
	[MUL_RBX] = {
		.opcode = {0x48, 0xf7, 0xeb},
		.str = "imul %rbx",
		.opcode_size = 3,
		.payload_address = false,
		.payload_value = false,
	},
	[CLEAR_RDX] = {
		.opcode = {0x99},
		.str = "cldt",
		.opcode_size = 1,
		.payload_address = false,
		.payload_value = false,
	},
	[DIV_RBX] = {
		.opcode = {0x48, 0xf7, 0xfb},
		.str = "idiv %rbx",
		.opcode_size = 3,
		.payload_address = false,
		.payload_value = false,
	},
	[POP_RAX] = {
		.opcode = {0x58},
		.str = "pop %rax",
		.opcode_size = 1,
		.payload_address = false,
		.payload_value = false,
	},
	[POP_RBX] = {
		.opcode = {0x5b},
		.str = "pop %rbx",
		.opcode_size = 1,
		.payload_address = false,
		.payload_value = false,
	},
	[POP_RDI] = {
		.opcode = {0x5f},
		.str = "pop %rdi",
		.opcode_size = 1,
		.payload_address = false,
		.payload_value = false,
	},
	[POP_RSI] = {
		.opcode = {0x5e},
		.str = "pop %rsi",
		.opcode_size = 1,
		.payload_address = false,
		.payload_value = false,
	},
	[PUSH_RAX] = {
		.opcode = {0x50},
		.str = "push %rax",
		.opcode_size = 1,
		.payload_address = false,
		.payload_value = false,
	},
	[PUSH_RBX] = {
		.opcode = {0x53},
		.str = "push %rbx",
		.opcode_size = 1,
		.payload_address = false,
		.payload_value = false,
	},
	[PUSH_RCX] = {
		.opcode = {0x51},
		.str = "push %rcx",
		.opcode_size = 1,
		.payload_address = false,
		.payload_value = false,
	},
	[MOV_RAX_RDI] = {
		.opcode = {0x48, 0x89, 0xc7},
		.str = "mov %rax, %rdi",
		.opcode_size = 3,
		.payload_address = false,
		.payload_value = false,
	},
	[MOV_RDX_RAX] = {
		.opcode = {0x48, 0x89, 0xd0},
		.str = "mov %rdx, %rax",
		.opcode_size = 3,
		.payload_address = false,
		.payload_value = false,
	},
	[MOV_IMM_RAX] = {
		.opcode = {0x48, 0xc7, 0xc0},
		.str = "mov $0x%lx, %rax",
		.opcode_size = 3,
		.payload_address = false,
		.payload_value = true,
		.payload_size = 4,
	},
	[MOV_IMM_RCX_LARGE] = {
		.opcode = {0x48, 0xb9},
		.str = "mov $0x%lx, %rcx",
		.opcode_size = 2,
		.payload_address = true,
		.payload_value = false,
		.payload_size = 8,
	},
	[CALL_RCX] = {
		.opcode = {0xff, 0xd1},
		.str = "call *%rcx",
		.opcode_size = 2,
		.payload_address = false,
		.payload_value = false,
	},
	[PUSH_RBP] = {
		.opcode = {0x55},
		.str = "push %rbp",
		.opcode_size = 1,
		.payload_address = false,
		.payload_value = false,
	},
	[MOV_RSP_RBP] = {
		.opcode = {0x48, 0x89, 0xe5},
		.str = "mov %rsp, %rbp",
		.opcode_size = 3,
		.payload_address = false,
		.payload_value = false,
	},
	[LEAVE] = {
		.opcode = {0xc9},
		.str = "leave",
		.opcode_size = 1,
		.payload_address = false,
		.payload_value = false,
	},
	[RET] = {
		.opcode = {0xc3},
		.str = "ret",
		.opcode_size = 1,
		.payload_address = false,
		.payload_value = false,
	},
};

static uint8_t *instruction_unknown(enum instruction_e instr,
				    libjit_value value, uint8_t *offset)
{
	(void)instr;
	(void)value;
	(void)offset;

	LIBJIT_DIE("Instruction %d unknown.", instr);
	return NULL;
}

static uint8_t *write_address(uint8_t *offset, void *addr)
{
	*(size_t **)offset = addr;
	return offset + sizeof(size_t);
}

static uint8_t *write_value(uint8_t *offset, libjit_value value, size_t size)
{
	uint8_t *ptr = (uint8_t *)&value;
	for (size_t i = 0; i < size; i++)
		offset[i] = ptr[i];

	return offset + size;
}

static uint8_t *write_instruction(enum instruction_e instr, libjit_value value,
				  uint8_t *offset)
{
	if (instr >= INSTR_COUNT)
		return instruction_unknown(instr, value, offset);

	for (size_t i = 0; i < instructions[instr].opcode_size; i++, offset++)
		*offset = instructions[instr].opcode[i];

	if (instructions[instr].payload_address)
		return write_address(offset, (void *)value);
	if (instructions[instr].payload_value)
		return write_value(offset, value,
				   instructions[instr].payload_size);

	return offset;
}

typedef uint8_t *(*operation_handler)(enum operation operation,
				      libjit_value value, uint8_t *offset);

static enum instruction_e simple_operation_mapping[] = {
	[OPER_ADD] = ADD_RBX_RAX,
	[OPER_SUB] = SUB_RBX_RAX,
	[OPER_MULT] = MUL_RBX,
	[OPER_PUSH_A] = PUSH_RAX,
	[OPER_PUSH_B] = PUSH_RBX,
	[OPER_POP_A] = POP_RAX,
	[OPER_POP_B] = POP_RBX,
	[OPER_POP_PARAM1] = POP_RDI,
	[OPER_POP_PARAM2] = POP_RSI,
	[OPER_RET] = RET,
	[OPER_FUNCTION_EPILOGUE] = LEAVE,
};

static uint8_t *simple_operation_handler(enum operation operation,
					 libjit_value value, uint8_t *offset)
{
	return write_instruction(simple_operation_mapping[operation], value,
				 offset);
}

static uint8_t *handle_division(enum operation operation, libjit_value value,
				uint8_t *offset)
{
	(void)operation;
	(void)value;

	offset = write_instruction(CLEAR_RDX, NO_VALUE, offset);
	return write_instruction(DIV_RBX, NO_VALUE, offset);
}

static uint8_t *handle_mod(enum operation operation, libjit_value value,
				uint8_t *offset)
{
	(void)operation;
	(void)value;

	offset = write_instruction(CLEAR_RDX, NO_VALUE, offset);
	offset = write_instruction(DIV_RBX, NO_VALUE, offset);
	return write_instruction(MOV_RDX_RAX, NO_VALUE, offset);
}

static uint8_t *handle_call(enum operation operation, libjit_value value,
			    uint8_t *offset)
{
	(void)operation;

	offset = write_instruction(MOV_IMM_RCX_LARGE, value, offset);
	return write_instruction(CALL_RCX, NO_VALUE, offset);
}

static uint8_t *handle_push_imm(enum operation operation, libjit_value value,
				uint8_t *offset)
{
	(void)operation;

	offset = write_instruction(MOV_IMM_RCX_LARGE, value, offset);
	return write_instruction(PUSH_RCX, NO_VALUE, offset);
}

static uint8_t *handle_function_prologue(enum operation operation,
					 libjit_value value, uint8_t *offset)
{
	(void)operation;
	(void)value;

	offset = write_instruction(PUSH_RBP, NO_VALUE, offset);
	return write_instruction(MOV_RSP_RBP, NO_VALUE, offset);
}

static operation_handler operation_handlers[] = {
	[SIMPLE_OPER_BEGIN... SIMPLE_OPER_END] = simple_operation_handler,
	[OPER_DIV] = handle_division,
	[OPER_MOD] = handle_mod,
	[OPER_PUSH_IMM] = handle_push_imm,
	[OPER_FUNCTION_PROLOGUE] = handle_function_prologue,
	[OPER_CALL] = handle_call,
};

uint8_t *write_operation(enum operation operation, size_t value,
			 uint8_t *offset)
{
	ASSERT(operation < OPER_COUNT, "Operation %d doesn't exist.",
	       operation);
	return operation_handlers[operation](operation, value, offset);
}

void dump_instructions(uint8_t *offset, uint8_t *end_offset)
{
	while (offset < end_offset) {
		bool found = false;
		for (enum instruction_e instr = ADD_RBX_RAX;
		     instr < INSTR_COUNT && !found; instr++) {
			struct instruction *instruction = instructions + instr;
			if (offset + instruction->opcode_size +
				    instruction->payload_size >
			    end_offset)
				continue;

			if (memcmp(offset, instruction->opcode,
				   instruction->opcode_size) != 0)
				continue;

			offset += instruction->opcode_size;
			if (!instruction->payload_address &&
			    !instruction->payload_value)
				printf("%s\n", instruction->str);
			else {
				printf(instruction->str, *(size_t *)(offset));
				printf("\n");
				offset += sizeof(size_t);
			}

			found = true;
		}
		ASSERT(found, "Couldn't disassemble an instruction.\n");
	}
}
