#include "instruction_database.h"
#include "log_messages.h"
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <assert.h>

INSTRUCTION_DATABASE *create_database(char* filename) {
	size_t file_size;
	struct stat st;
	int status;
	status = stat(filename, &st);
	if(status != 0){
		ERROR_LOG("Error loading instruction database: %s\n", filename);
		return NULL;
	}
	file_size = st.st_size;
	if(file_size == 0){
		ERROR_LOG("Translation database is empty: %s\n", filename);
		return NULL;	
	}
	int fd = open(filename, O_RDWR);
	if (fd == -1) {
		ERROR_LOG("Translation database open failed: %s\n", filename);
		return NULL;
	}
	char* base_address;
	base_address = mmap(NULL, file_size, PROT_READ, MAP_SHARED, fd, 0);
	if (base_address == NULL){
		ERROR_LOG("Translation database mapping failed: %s\n", filename);
		return NULL;
	}
	INSTRUCTION_DATABASE* instruction_database = malloc(sizeof(*instruction_database));
	assert(strlen(filename) < sizeof(instruction_database->file_name) && "file name too long to fit in header");
	strcpy(instruction_database->file_name, filename);
	instruction_database->fd = fd;
	instruction_database->base_address = base_address;
	
	//Find text section and start reading

	
	int number_of_sections = *(unsigned short*)(base_address + 2);
	char* symbol_table_address = base_address + *(unsigned int*)(base_address + 8);
	int symbol_count = *(unsigned int*)(base_address+ 12);
	char* string_table_address = symbol_table_address+ 18 * symbol_count;
	int size_of_optional_headers = *(unsigned short*)(base_address + 16);
	char* section_table_address = (char*)base_address + 20 + size_of_optional_headers;
	int i;
	instruction_database->symbol_table_address = symbol_table_address;
	instruction_database->symbol_count = symbol_count;
	instruction_database->string_table_address = string_table_address;
	char* text_section_address = NULL;
	for (i=0;i<number_of_sections;i++){
		char *header_address = 40*i + section_table_address;
		char section_name[256];
		char name[9];
		name[8] = 0;
		memcpy(name, header_address, 8);
		if(name[0] == '/'){
			int string_table_index = strtol(name+1, NULL, 10);
			strcpy(section_name, string_table_address + string_table_index);
		}else{
			strcpy(section_name, name);
		}
		if(strcmp(".text", section_name) == 0){
			text_section_address = (char*)base_address + *(unsigned int*)(header_address + 20);
			break;
		}

	}
	if(text_section_address == NULL) {
		ERROR_LOG(".text section not found in translation database: %s\n", filename);
		return NULL;
	}
	instruction_database->text_section_address = text_section_address;
	for(i = 0; i<256; i++)
		instruction_database->instruction_translation_start[i] = NULL;
	for(i = 0; i<symbol_count;i++) {
		char *header_address = 18*i + symbol_table_address;
		char symbol_name[256];
		char name[9];
		name[8] = 0;
		memcpy(name, header_address, 8);
		if(name[0] == 0 && name[1] == 0 && name[2] == 0	&& name[3] == 0){
			int string_table_index = *(unsigned int*)((unsigned char*) name + 4);
			strcpy(symbol_name, string_table_address + string_table_index);
		}else{
			strcpy(symbol_name, name);
		}
		int opcode;
		if(sscanf(symbol_name, "NES_INSTRUCTION_0x%2x", &opcode) == 1){
			void* instruction_address = text_section_address + *(unsigned int*) (header_address + 8);
			instruction_database->instruction_translation_start[opcode] = instruction_address;
		} 
	}
	for(i=0;i<256;i++){
		if(instruction_database->instruction_translation_start[i] == NULL){
			ERROR_LOG("No address resolved for opcode %x in database\n", i);
			return NULL;
		}
	}
	return instruction_database;
}
