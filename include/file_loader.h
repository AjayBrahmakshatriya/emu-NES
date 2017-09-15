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

#define SIZE_OF_HEADERS sizeof(FILE_HEADERS)

#define SIZE_OF_TRAINER 0x200

#define FILE_HEADERS_TRAINER_EXISTS (1 << 0x2)


typedef struct {
	char file_path[FILE_PATH_SIZE];
	int fd;
	void* image_base_address;
	size_t image_size;
	FILE_HEADERS* file_headers;
	void *start_of_prg;
	size_t size_of_prg;
	void* address_space_start;
} FILE_HANDLE, *PFILE_HANDLE;


FILE_HANDLE* load_file(const char* filename, void* base_address);
void close_file(FILE_HANDLE *file_handle);
int map_file(FILE_HANDLE *file_handle, void *address_space_start);
int unmap_file(FILE_HANDLE *file_handle);
void* identify_reset_address(FILE_HANDLE *file_handle);
unsigned long long identify_reset_address_emulation(FILE_HANDLE *file_handle);

void* translate_address_to_emulation_context(FILE_HANDLE *file_handle, unsigned short address);

#endif

