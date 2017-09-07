#include "nes_decoder.h"
#include "log_messages.h"

const NES_INSTRUCTION* decode_address(void *address, int* opcode, int* argument, int *size){
	int decoded_opcode = (int)*(unsigned char*)address;
	*opcode = decoded_opcode;
	*size = AM_sizes[nes_instructions[decoded_opcode].addressing_mode];
	*argument = 0;
	switch(*size) {
		case 1:
			break;
		case 2:	
			*argument += ((unsigned char*)address)[1];
			break;
		case 3:
			*argument += ((unsigned char*)address)[1] + ((int)(((unsigned char*)address)[2])<<8);
			break;
		default:
			ERROR_LOG("Invalid size for decoding = %d\n", *size);
	}
	return &nes_instructions[decoded_opcode];
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
		int opcode;
		int size;
		int argument;
		decoded_instruction = decode_address(address, &opcode, &argument, &size);
		NES_ADDRESSING_MODE am = decoded_instruction->addressing_mode;
		if (decoded_instruction->name == NULL) {
			ERROR_LOG("Invalid instruction(%x) at %p\n", opcode, address);
			return;
		}
		printf("%08x: %s %s %04x\n", (int)(address-base_address), decoded_instruction->name, AM_names[am], argument); 
		address += size;
		printed += size;
		if(is_bb_end_opcode(opcode))
			break;
	}
}
