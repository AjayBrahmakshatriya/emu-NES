#ifndef NES_DECODER
#define NES_DECODER
#include <stdlib.h>

typedef struct {
	const char* name;
	int size;
	int cycles;
} NES_INSTRUCTION;


extern NES_INSTRUCTION nes_instructions[256];


const NES_INSTRUCTION* decode_address(void* address);
void decode_and_print_from(void *address, size_t to_print, void* base_address);

#endif


