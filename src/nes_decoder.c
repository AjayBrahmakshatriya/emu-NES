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
		if (decoded_instruction->name == NULL) {
			ERROR_LOG("Invalid instruction at %p\n", address);
			return;
		}
		printf("%8x: %s\n", (int)(address-base_address), decoded_instruction->name); 
		address += decoded_instruction->size;
		printed += decoded_instruction->size; 
	}
}
