#include "execution_context.h"
#include "nes_decoder.h"
#include "log_messages.h"
#include <stdlib.h>
#include <string.h>
#include "nes_decoder.h"


void intialize_emulation_vector(EMULATION_VECTOR *emulation_vector) {
	emulation_vector->basic_block_end = basic_block_end;
}


void initialize_execution_context(EXECUTION_CONTEXT *execution_context, FILE_HANDLE *file_handle, EXECUTION_AREA *execution_area, INSTRUCTION_DATABASE *instruction_database) {
	execution_context->file_handle = file_handle;
	execution_context->execution_area = execution_area;
	execution_context->instruction_database = instruction_database;
	execution_context->registers.A = 0x0;
	execution_context->registers.X = 0x0;
	execution_context->registers.Y = 0x0;
	execution_context->registers.S = 0xFF;
	execution_context->registers.SR = 0x0;
	execution_context->registers.emulation_vector = &(execution_context->emulation_vector);
	execution_context->registers.base_address = file_handle->address_space_start;
	intialize_emulation_vector(&(execution_context->emulation_vector));
}



void *generate_basic_block(EXECUTION_CONTEXT *execution_context, unsigned long long address) {
	INFO_LOG("Translating BB at = %4x\n", (int)address);
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
		long long offset;
		switch(decoded_instruction->addressing_mode) {
			case AM_abs:
				offset = find_arg_location(execution_context->instruction_database, opcode, 0);
				if(offset < 0){
					WARN_LOG("Arg 0 not found for %s %s\n", decoded_instruction->name, AM_names[decoded_instruction->addressing_mode]);
				}else{
					memcpy(virtual_address_assigned + offset, &argument, 2);
				}
				break;
			case AM_A:
			case AM_abs_X:
			case AM_abs_Y:
			case AM_hash:
			case AM_impl:
			case AM_ind:
			case AM_X_ind:
			case AM_ind_Y:
			case AM_rel:
			case AM_zpg:
			case AM_zpg_X:
			case AM_zpg_Y:
				break;
			default:
				ERROR_LOG("Invalid addressing mode = %d\n", decoded_instruction->addressing_mode);
 
		}

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
void print_next_address(EXECUTION_CONTEXT *execution_context, unsigned long long next_address) {
	INFO_LOG("Execution continuing to = 0x%04x\n", (int)next_address);
	return;
}
