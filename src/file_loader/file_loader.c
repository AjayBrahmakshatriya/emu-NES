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
	file_handle->image_base_address = base_address;
	file_handle->image_size = file_size;
	file_handle->file_headers = base_address;
	if(validate_headers(file_handle->file_headers))
		INFO_LOG("File headers correctly verified\n");
	else {
		ERROR_LOG("File headers verification failed\n");
		free(file_handle);
		return NULL;
	}
	
	return file_handle;
}


void close_file(FILE_HANDLE* file_handle) {
	if(file_handle) {
		/* 
			Code to clear up file
		*/
	}
}
