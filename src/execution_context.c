#include "execution_context.h"
#include "nes_decoder.h"
#include "log_messages.h"
#include <stdlib.h>
#include <string.h>
#include "nes_decoder.h"


void initialize_execution_context(EXECUTION_CONTEXT *execution_context, FILE_HANDLE *file_handle, EXECUTION_AREA *execution_area, INSTRUCTION_DATABASE *instruction_database) {
	execution_context->file_handle = file_handle;
	execution_context->execution_area = execution_area;
	execution_context->instruction_database = instruction_database;
	execution_context->registers.A = 0x0;
	execution_context->registers.X = 0x0;
	execution_context->registers.Y = 0x0;
	execution_context->registers.S = 0xFF;
	execution_context->registers.SR = 0x0;
}



void *generate_basic_block(EXECUTION_CONTEXT *execution_context, unsigned long long address) {
	size_t translated = 0;
	void *to_decode_address = translate_address_to_emulation_context(execution_context->file_handle, address);
	const NES_INSTRUCTION *decoded_instruction = NULL;
	int opcode;
	int size;
	int argument;
	while(1) {
		decoded_instruction = decode_address(to_decode_address, &opcode, &argument, &size);
		if (decoded_instruction->name == NULL) {
			ERROR_LOG("Invalid instruction (%x) at %p\n", opcode, to_decode_address);
			return NULL;
		}
		void* start_address = find_instruction_start(execution_context->instruction_database, opcode);
		void* end_address = find_instruction_start(execution_context->instruction_database, opcode+1);
		if(start_address == NULL || end_address == NULL || (unsigned long long)start_address >= (unsigned long long)end_address){
			ERROR_LOG("Bad translation for opcode %x\n", opcode);
			return NULL;
		} 	
		size_t translation_size = end_address - start_address;
		void* virtual_address_assigned = allocate_address(execution_context->execution_area, translation_size);
		if(virtual_address_assigned == NULL)
			return NULL;
		memcpy(virtual_address_assigned, start_address, translation_size);
		execution_context->execution_area->address_map->address_map[address+translated] = virtual_address_assigned;
		translated += size;
		to_decode_address += size;
		if(is_bb_end_opcode(opcode))
			break;
	} 
	return execution_context->execution_area->address_map->address_map[address];
	
}

void* get_execution_address(EXECUTION_CONTEXT *execution_context, unsigned long long address) {
	if (execution_context->execution_area->address_map->address_map[address] != NULL)
		return execution_context->execution_area->address_map->address_map[address];
	void* return_address = generate_basic_block(execution_context, address);
	return return_address;
}