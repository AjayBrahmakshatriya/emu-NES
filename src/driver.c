#include "file_loader.h"
#include "log_messages.h"
#include "nes_decoder.h"
#include "instruction_database.h"

#include <stdlib.h>


#define IMAGE_BASE (void*) 0x300000000ULL
#define ADDRESS_SPACE_START (void*) 0x400000000ULL



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
	
	decode_and_print_from(file_handle, execution_start_address, -1);
	
	unmap_file(file_handle);	
	close_file(file_handle);
	
	return 0;
}
