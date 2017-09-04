#include "file_loader.h"
#include "log_messages.h"

#include <stdlib.h>


#define IMAGE_BASE (void*) 0x300000000ULL



int main(int argc, char *argv[]) {
	if(argc < 2) {
		ERROR_LOG("Insufficient arguments, please call as %s <nes-rom-filename>\n", argv[0]);
		return -1;
	}
	INFO_LOG("File name = %s\n", argv[1]);
	FILE_HANDLE *file_handle = load_file(argv[1], IMAGE_BASE);

	close_file(file_handle);

	return 0;
}
