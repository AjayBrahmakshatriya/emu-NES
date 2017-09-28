#include "file_loader.h"
#include "log_messages.h"
#include "nes_decoder.h"
#include "instruction_database.h"
#include "execution_area.h"
#include "execution_context.h"
#include "ppu.h"

#include <stdlib.h>


#define IMAGE_BASE (void*) 0x300000000ULL
#define ADDRESS_SPACE_START (void*) 0x400000000ULL
#define EXECUTION_AREA_START (void*) 0x500000000ULL


int main(int argc, char *argv[]) {
	if(argc < 3) {
		ERROR_LOG("Insufficient arguments, please call as %s <nes-rom-filename> <instruction-database>\n", argv[0]);
		return -1;
	}
	INFO_LOG("File name = %s\n", argv[1]);
	FILE_HANDLE *file_handle = load_file(argv[1], IMAGE_BASE);
	if(file_handle == NULL)
		return -1;
	if(map_file(file_handle, ADDRESS_SPACE_START) == 0) {
		INFO_LOG("File mapping completed at address = %p\n", file_handle->address_space_start);
	}else{
		close_file(file_handle);
		return -1;
	}
	void *execution_start_address = identify_reset_address(file_handle);
	INFO_LOG("Execution to start at %p\n", execution_start_address);
	INSTRUCTION_DATABASE *instruction_database = create_database(argv[2]);
	EXECUTION_AREA *execution_area = create_execution_area(EXECUTION_AREA_START);
	PPU *ppu = create_ppu(file_handle);

	EXECUTION_CONTEXT execution_context;
	initialize_execution_context(&execution_context, file_handle, execution_area, instruction_database, ppu);
	if(argc == 3 )
		start_execution(&execution_context);	
	else{
		unsigned int address;
		sscanf(argv[3], "%i", &address);
		INFO_LOG("Address decompiling at = %x\n", address);	
		decode_and_print_from(file_handle, execution_start_address - 0x8000 + address, -1);
	}
	destroy_ppu(ppu);
	destroy_execution_area(execution_area);
	unmap_file(file_handle);	
	close_file(file_handle);
	
	return 0;
}
