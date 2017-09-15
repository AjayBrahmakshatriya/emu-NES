#include "file_loader.h"
#include "log_messages.h"

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>


#include <assert.h>

#define ERR_OK 0

int validate_headers(FILE_HEADERS *headers) {
	if(!(headers->constant[0] == 0x4E && headers->constant[1] == 0x45 && headers->constant[2] == 0x53 && headers->constant[3] == 0x1A))
		return 0;
	return 1;
}

unsigned char identify_mapper(FILE_HEADERS *headers) {
	return ((headers->flags_6 & 0xF0) >> 4) | (headers->flags_7 & 0xF0);
}


FILE_HANDLE* load_file(const char* filename, void* base_address) {
	if(base_address == NULL)
		return NULL;
	size_t file_size;
	struct stat st;
	int status;
	status = stat(filename, &st);
	if (status != ERR_OK) {
		ERROR_LOG("Error loading file: %s\n", filename);
		return NULL;
	}
	file_size = st.st_size;
	if (file_size == 0){
		ERROR_LOG("NES file empty: %s\n", filename);
		return NULL;
	}
	int fd = open(filename, O_RDWR);
	if (fd == -1) {
		ERROR_LOG("NES file open failed: %s\n", filename);
		return NULL;
	}
	if(base_address != mmap(base_address, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)){
		ERROR_LOG("NES file mapping failed: %s\n", filename);
		return NULL;
	}
	FILE_HANDLE *file_handle = malloc(sizeof(*file_handle));
	assert(strlen(filename) < sizeof(file_handle->file_path) && "file name too long to fit in header");
	strcpy(file_handle->file_path, filename);
	file_handle->fd = fd;
	file_handle->image_base_address = base_address;
	file_handle->image_size = file_size;
	file_handle->file_headers = base_address;
	
	if(validate_headers(file_handle->file_headers)) {
		INFO_LOG("File headers correctly verified\n");
	}else {
		ERROR_LOG("File headers verification failed\n");
		free(file_handle);
		return NULL;
	}
	if (file_handle->file_headers->flags_6 & FILE_HEADERS_TRAINER_EXISTS){
		file_handle->start_of_prg = base_address + SIZE_OF_HEADERS + SIZE_OF_TRAINER;	
	}else{
		file_handle->start_of_prg = base_address + SIZE_OF_HEADERS;
	}
	file_handle->size_of_prg = (size_t)file_handle->file_headers->prg_rom_size * 0x4000;
	INFO_LOG("Size of game code is %zu bytes\n", file_handle->size_of_prg);
	return file_handle;
}


#define INES_MAPPER_000 0

int map_file(FILE_HANDLE *file_handle, void *address_space_start){
	if(file_handle == NULL)
		return -1;

	if(address_space_start != mmap(address_space_start, 0x10000, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)){
		ERROR_LOG("Address space creation failed for %s\n", file_handle->file_path);
		return -1;
	}
	file_handle->address_space_start = address_space_start;
	
	unsigned char mapper_id = identify_mapper(file_handle->file_headers);
	switch(mapper_id){
		case INES_MAPPER_000: // Refer https://wiki.nesdev.com/w/index.php?title=INES_Mapper_000
			memset((char*)address_space_start + 0x6000, 0, 0x2000);
			memcpy((char*)address_space_start + 0x8000, file_handle->start_of_prg, 0x4000);
			if(file_handle->size_of_prg > 0x4000)
				memcpy((char*)address_space_start + 0xC000, (char*)file_handle->start_of_prg + 0x4000, 0x4000);
			else	
				memcpy((char*)address_space_start + 0xC000, (char*)file_handle->start_of_prg, 0x4000);
			return 0;
		default:		
			ERROR_LOG("Unsupported mapper = %d\n", mapper_id);
			return -1;
	}
	
}

int unmap_file(FILE_HANDLE *file_handle) {
	if(file_handle == NULL)
		return -1;	
	if(file_handle->address_space_start == NULL)
		return -1;
	/* Code to unmap address space */
	return 0;
}

void close_file(FILE_HANDLE* file_handle) {
	if(file_handle == NULL)
		return;
	/* 
		Code to clear up file
	*/
	return;
}

unsigned long long identify_reset_address_emulation(FILE_HANDLE *file_handle){
	unsigned char *reset_address_location = translate_address_to_emulation_context(file_handle, 0xFFFC);
	unsigned short address = (unsigned short)reset_address_location[0] | ((unsigned short)reset_address_location[1] << 8);
	return address;
}

void* identify_reset_address(FILE_HANDLE *file_handle) {
	return translate_address_to_emulation_context(file_handle, identify_reset_address_emulation(file_handle));
}

void* translate_address_to_emulation_context(FILE_HANDLE *file_handle, unsigned short address) {
	return (char*)file_handle->address_space_start + address;
}
