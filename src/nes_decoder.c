#include "nes_decoder.h"
#include "log_messages.h"

const NES_INSTRUCTION* decode_address(void *address){
	int opcode = (int)*(unsigned char*)address;
	return &nes_instructions[opcode];
}

int is_bb_end_opcode(int opcode){
	int i;
	for(i=0;i< BB_END_OPCODES_COUNT; i++)
		if (opcode == BB_END_OPCODES[i])
			return 1;
	return 0;
}


void decode_and_print_from(FILE_HANDLE *file_handle, void *address, size_t to_print) {
	if(to_print == -1)
		to_print = 0xFFFF;
	size_t printed = 0;
	void* base_address = file_handle->address_space_start;
	const NES_INSTRUCTION *decoded_instruction = NULL;
	while(printed < to_print) {
		decoded_instruction = decode_address(address);
		NES_ADDRESSING_MODE am = decoded_instruction->addressing_mode;
		int opcode = *(unsigned char*) address;

		if (decoded_instruction->name == NULL) {
			ERROR_LOG("Invalid instruction(%x) at %p\n", opcode, address);
			return;
		}
		printf("%8x: %s %s\n", (int)(address-base_address), decoded_instruction->name, AM_names[am]); 
		address += AM_sizes[am];
		printed += AM_sizes[am]; 
		if(is_bb_end_opcode(opcode))
			break;
	}
}
