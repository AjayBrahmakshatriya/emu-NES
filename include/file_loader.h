#ifndef FILE_LOADER_H
#define FILE_LOADER_H

#define FILE_PATH_SIZE 256

#include <stdlib.h>

 typedef struct __attribute__((packed)) {
	unsigned char constant[4]; 	/* Should be 4E 45 53 1A */ 
	unsigned char prg_rom_size;	/* in 16 KB units 	 */
	unsigned char chr_rom_size;	/* in 08 KB units	 */
	unsigned char flags_6;		
	unsigned char flags_7;
	unsigned char prg_ram_size;	/* in 08 KB units	 */
	unsigned char flags_9;
	unsigned char flags_10;
	unsigned char zeros[5];
}FILE_HEADERS, *PFILE_HEADERS;


#define FILE_HEADERS_TRAINER_EXISTS 0x2


typedef struct {
	char file_path[FILE_PATH_SIZE];
	void* image_base_address;
	size_t image_size;
	FILE_HEADERS* file_headers;
} FILE_HANDLE, *PFILE_HANDLE;


FILE_HANDLE* load_file(const char* filename, void* base_address);
void close_file(FILE_HANDLE *file_handle);
#endif

