#include "nes_decoder.h"
#include "log_messages.h"

const NES_INSTRUCTION* decode_address(void *address){
	int opcode = (int)*(unsigned char*)address;
	return &nes_instructions[opcode];
}


void decode_and_print_from(void *address, size_t to_print, void* base_address) {
	size_t printed = 0;
	const NES_INSTRUCTION *decoded_instruction = NULL;
	while(printed < to_print) {
		decoded_instruction = decode_address(address);
		NES_ADDRESSING_MODE am = decoded_instruction->addressing_mode;
		if (decoded_instruction->name == NULL) {
			ERROR_LOG("Invalid instruction(%x) at %p\n", *(unsigned char*)address, address);
			return;
		}
		printf("%8x: %s %s\n", (int)(address-base_address), decoded_instruction->name, AM_names[am]); 
		address += AM_sizes[am];
		printed += AM_sizes[am]; 
	}
}
