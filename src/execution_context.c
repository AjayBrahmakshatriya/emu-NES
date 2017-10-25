#include "execution_context.h"
#include "nes_decoder.h"
#include "log_messages.h"
#include <stdlib.h>
#include <string.h>
#include "nes_decoder.h"
#include "ppu.h"

void intialize_emulation_vector(EMULATION_VECTOR *emulation_vector) {
	emulation_vector->basic_block_end = basic_block_end;
	emulation_vector->read_non_ram_address = read_non_ram_address;
	emulation_vector->write_non_ram_address = write_non_ram_address;
	emulation_vector->ppu_event = ppu_event;
	emulation_vector->nmi_trigger = nmi_trigger;
}


void initialize_execution_context(EXECUTION_CONTEXT *execution_context, FILE_HANDLE *file_handle, EXECUTION_AREA *execution_area, INSTRUCTION_DATABASE *instruction_database, PPU *ppu, NES_DISPLAY *nes_display) {
	execution_context->file_handle = file_handle;
	execution_context->execution_area = execution_area;
	execution_context->instruction_database = instruction_database;
	execution_context->ppu = ppu;
	ppu->execution_context = execution_context;
	execution_context->nes_display = nes_display;
	reset_ppu(ppu);
	execution_context->nmi_flip_flop = 0;
	execution_context->interrupts_disabled = 0;
	execution_context->registers.A = 0x0;
	execution_context->registers.X = 0x0;
	execution_context->registers.Y = 0x0;
	execution_context->registers.S = 0xFF;
	execution_context->registers.SR = 0x0;
	execution_context->registers.emulation_vector = &(execution_context->emulation_vector);
	execution_context->registers.base_address = file_handle->address_space_start;
	intialize_emulation_vector(&(execution_context->emulation_vector));
}


void write_argument(EXECUTION_CONTEXT *execution_context, const NES_INSTRUCTION *decoded_instruction, int opcode, int arg_n, char* virtual_address_assigned, unsigned int argument, unsigned short argument_size){
	long long offset; 
	offset = find_arg_location(execution_context->instruction_database, opcode, arg_n);
	if(offset < 0){
		if(arg_n!=-1)
			WARN_LOG("Arg %d not found for %s %s\n",arg_n, decoded_instruction->name, AM_names[decoded_instruction->addressing_mode]);
	}else{
		memcpy(virtual_address_assigned + offset, &argument, argument_size);
	}
}
void *generate_basic_block(EXECUTION_CONTEXT *execution_context, unsigned long long address) {
	INFO_LOG("Translating BB at = %04x\n", (int)address);
	size_t translated = 0;
	void *to_decode_address = translate_address_to_emulation_context(execution_context->file_handle, address);
	const NES_INSTRUCTION *decoded_instruction = NULL;
	int opcode;
	int size;
	int argument;
	while(1) {
		decoded_instruction = decode_address(to_decode_address, &opcode, &argument, &size);
		if (decoded_instruction->name == NULL) {
			WARN_LOG("Invalid instruction (%x) at %p\n", opcode, to_decode_address);
			break;
		}

		void* virtual_address_assigned_test = allocate_address(execution_context->execution_area, execution_context->instruction_database->ppu_event_test_size);
		memcpy(virtual_address_assigned_test, execution_context->instruction_database->ppu_event_test, execution_context->instruction_database->ppu_event_test_size);
		write_argument(execution_context, decoded_instruction, 256, 0, virtual_address_assigned_test, decoded_instruction->base_cycles * 3, 1);
		write_argument(execution_context, decoded_instruction, 256, -2, virtual_address_assigned_test, address+translated, 2);


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
				write_argument(execution_context, decoded_instruction, opcode, 0, virtual_address_assigned, argument, 2);	
				break;
			case AM_A:
				break;
			case AM_abs_X:
				write_argument(execution_context, decoded_instruction, opcode, 0, virtual_address_assigned, argument, 2);	
				break;
			case AM_abs_Y:
				write_argument(execution_context, decoded_instruction, opcode, 0, virtual_address_assigned, argument, 2);
				break;	
			case AM_hash:
				write_argument(execution_context, decoded_instruction, opcode, 0, virtual_address_assigned, argument, 1);
				break;
			case AM_impl:
				break;
			case AM_ind:
				write_argument(execution_context, decoded_instruction, opcode, 0, virtual_address_assigned, argument, 2);
				break;
			case AM_X_ind:
				write_argument(execution_context, decoded_instruction, opcode, 0, virtual_address_assigned, argument, 1);
				break;
			case AM_ind_Y:
				write_argument(execution_context, decoded_instruction, opcode, 0, virtual_address_assigned, argument, 1);
				break;
			case AM_rel:
				write_argument(execution_context, decoded_instruction, opcode, 0, virtual_address_assigned, argument, 1);
				break;
			case AM_zpg:
				write_argument(execution_context, decoded_instruction, opcode, 0, virtual_address_assigned, argument, 1);
				break;
			case AM_zpg_X:
				write_argument(execution_context, decoded_instruction, opcode, 0, virtual_address_assigned, argument, 1);
				break;
			case AM_zpg_Y:
				write_argument(execution_context, decoded_instruction, opcode, 0, virtual_address_assigned, argument, 1);
				break;
			default:
				ERROR_LOG("Invalid addressing mode = %d\n", decoded_instruction->addressing_mode);
 
		}
		write_argument(execution_context, decoded_instruction, opcode, -1, virtual_address_assigned, address+translated+size, 2);
	/*	if(address + translated == 0xE427){
			virtual_address_assigned_test = allocate_address(execution_context->execution_area, 1);
			*(unsigned char*)virtual_address_assigned_test = 0xcc;
		}*/

		execution_context->execution_area->address_map->address_map[address+translated] = virtual_address_assigned_test;
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
	INFO_LOG("Execution continuing to = 0x%llx\n", (next_address));
	return;
}
unsigned char read_non_ram_address_internal(EXECUTION_CONTEXT *execution_context, unsigned short address) {
	
	if(address >= 0x2000 && address < 0x4000) {
		return ppu_read(execution_context->ppu, address);
	}else if(address == 0x4016 || address == 0x4017) {
		BYTE return_val;	
		if(address == 0x4016){
			switch(execution_context->controller_state.cycle1){
				case controller_A:	return_val = execution_context->nes_display->keypad1.A; break;
				case controller_B:	return_val = execution_context->nes_display->keypad1.B; break;
				case controller_select:	return_val = execution_context->nes_display->keypad1.select; break;
				case controller_start:	return_val = execution_context->nes_display->keypad1.start; break;
				case controller_up:	return_val = execution_context->nes_display->keypad1.up; break;
				case controller_down:	return_val = execution_context->nes_display->keypad1.down; break;
				case controller_left:	return_val = execution_context->nes_display->keypad1.left; break;
				case controller_right:	return_val = execution_context->nes_display->keypad1.right; break;
				case controller_loop:	ERROR_LOG("Controller enetered with loop cycle\n"); break;
			}
			if (!(execution_context->controller_state.input & 0b1)){
				execution_context->controller_state.cycle1++;
				if(execution_context->controller_state.cycle1 == controller_loop)
					execution_context->controller_state.cycle1 = controller_A;
			}
		}else{
			switch(execution_context->controller_state.cycle2){
				case controller_A:	return_val = execution_context->nes_display->keypad2.A; break;
				case controller_B:	return_val = execution_context->nes_display->keypad2.B; break;
				case controller_select:	return_val = execution_context->nes_display->keypad2.select; break;
				case controller_start:	return_val = execution_context->nes_display->keypad2.start; break;
				case controller_up:	return_val = execution_context->nes_display->keypad2.up; break;
				case controller_down:	return_val = execution_context->nes_display->keypad2.down; break;
				case controller_left:	return_val = execution_context->nes_display->keypad2.left; break;
				case controller_right:	return_val = execution_context->nes_display->keypad2.right; break;
				case controller_loop:	ERROR_LOG("Controller enetered with loop cycle\n"); break;
			}
			if (!(execution_context->controller_state.input & 0b1)){
				execution_context->controller_state.cycle2++;
				if(execution_context->controller_state.cycle2 == controller_loop)
					execution_context->controller_state.cycle2 = controller_A;
			}
		}
		return return_val;
	}if(address >= 0x800 && address < 0x1000) {
		return *(unsigned char*)translate_address_to_emulation_context(execution_context->file_handle, address);
	}
	else{
		ERROR_LOG("Invalid non ram read on address = %x\n", address);
		exit(-1);
	}
}
void write_non_ram_address_internal(EXECUTION_CONTEXT *execution_context, unsigned short address, unsigned char value) {	
	if((address >= 0x2000 && address < 0x4000) || address == 0x4014) {
		ppu_write(execution_context->ppu, address, value);
	}else if((address >=0x4000 && address < 0x4014) || address == 0x4015){
		// These are IO for APU. We will ignore for now
	}else if(address == 0x4016 || address == 0x4017){
		if(address == 0x4016){
			execution_context->controller_state.input = value;
			if(value & 0b1){
				execution_context->controller_state.cycle1 = controller_A;
				execution_context->controller_state.cycle2 = controller_A;
			}	
		}
	}
	else{
		ERROR_LOG("Invalid non ram write on address = %x\n", address);
		exit(-1);
	}
}
