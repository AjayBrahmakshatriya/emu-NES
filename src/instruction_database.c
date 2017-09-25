#include "instruction_database.h"
#include "log_messages.h"
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <assert.h>



void get_name_for_symbol(char symbol_name[256], char* symbol_table_address, int symbol_table_index, char *string_table_address){
	char name[9];
	name[8] = 0;
	char* header_address = symbol_table_address + symbol_table_index * 18;
	memcpy(name, header_address, 8);
	if(name[0] == 0 && name[1] == 0 && name[2] == 0	&& name[3] == 0){
		int string_table_index = *(unsigned int*)((unsigned char*) name + 4);
		strcpy(symbol_name, string_table_address + string_table_index);
	}else{
		strcpy(symbol_name, name);
	}
	return;
}
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
	char* text_relocation_address = NULL;
	int text_relocation_count = 0;

	for (i=0;i<number_of_sections;i++){
		char *header_address = 40*i + section_table_address;	
		char section_name[256];
		char name[9];
		name[8] = 0;
		memcpy(name, header_address , 8);
		if(name[0] == '/'){
			int string_table_index = strtol(name+1, NULL, 10);
			strcpy(section_name, string_table_address + string_table_index);
		}else{
			strcpy(section_name, name);
		}
		if(strcmp(".text", section_name) == 0){
			text_section_address = (char*)base_address + *(unsigned int*)(header_address + 20);
			text_relocation_address = (char*)base_address + *(unsigned int*)(header_address + 24);
			text_relocation_count = *(unsigned short*)(header_address + 32);
			break;
		}

	}
	if(text_section_address == NULL) {
		ERROR_LOG(".text section not found in translation database: %s\n", filename);
		return NULL;
	}
	instruction_database->text_section_address = text_section_address;
	instruction_database->text_relocation_address = text_relocation_address;
	instruction_database->text_relocation_count = text_relocation_count;
	void * ppu_event_test_end=NULL;
	instruction_database->ppu_event_test = NULL;
	INFO_LOG("%d relocations found at address = %p (%x)\n", text_relocation_count, text_relocation_address, (unsigned int)(text_relocation_address - base_address));
	for(i = 0; i<256; i++)
		instruction_database->instruction_translation_start[i] = NULL;
	for(i = 0; i<symbol_count;i++) {
		char *header_address = 18*i + symbol_table_address;
		char symbol_name[256];
		get_name_for_symbol(symbol_name, symbol_table_address, i, string_table_address);
		int opcode;
		if(sscanf(symbol_name, "NES_INSTRUCTION_0x%2x", &opcode) == 1){
			void* instruction_address = text_section_address + *(unsigned int*) (header_address + 8);
			instruction_database->instruction_translation_start[opcode] = instruction_address;
		}else if(strcmp(symbol_name, "NES_TEST_PPU_EVENT") == 0) {
			void* instruction_address = text_section_address + *(unsigned int*) (header_address + 8);
			instruction_database->ppu_event_test = instruction_address;
		}else if(strcmp(symbol_name, "NES_TEST_PPU_EVENT_END") == 0) {	
			void* instruction_address = text_section_address + *(unsigned int*) (header_address + 8);
			ppu_event_test_end = instruction_address;
		}
	}
	for(i=0;i<256;i++){
		if(instruction_database->instruction_translation_start[i] == NULL){
			ERROR_LOG("No address resolved for opcode %x in database\n", i);
			return NULL;
		}
	}
	if (instruction_database->ppu_event_test == NULL || ppu_event_test_end == NULL) {
			ERROR_LOG("No address resolved for NES_TEST_PPU_EVENT\n");
			return NULL;
	}else{
		instruction_database->ppu_event_test_size = ppu_event_test_end - instruction_database->ppu_event_test;
	}
	return instruction_database;
}


void* find_instruction_start(INSTRUCTION_DATABASE *instruction_database, int opcode) {
	return instruction_database->instruction_translation_start[opcode];
}

#define IMAGE_REL_AMD64_ADDR32 0x2
long long find_arg_location(INSTRUCTION_DATABASE *instruction_database, int opcode, int arg) {
	char opcode_str[3];

	if (opcode < 256) 
		sprintf(opcode_str, "%02x", opcode);
	else if(opcode == 256)
		sprintf(opcode_str, "pe");
	else{
		ERROR_LOG("Invalid opcode for finding args\n");
		return -1;
	}
	char arg_string[30];
	if(arg!=-1)
		sprintf(arg_string, "__arg_%s_%01x", opcode_str, arg);		
	else
		sprintf(arg_string, "__arg_%s_p", opcode_str);
	int i = 0;
	char* symbol_table_address = instruction_database->symbol_table_address;
	char* string_table_address = instruction_database->string_table_address;
	int symbol_table_count = instruction_database->symbol_count;
	char symbol_name[256];
	for(i=0;i<instruction_database->text_relocation_count;i++){
		char* header_address = instruction_database->text_relocation_address + 10 * i;
		int type = *(unsigned short*) (header_address + 8);
		if(type == IMAGE_REL_AMD64_ADDR32){
			int symbol_table_index = *(unsigned int*)(header_address + 4);
			get_name_for_symbol(symbol_name, symbol_table_address, symbol_table_index, string_table_address);
			if(strcmp(symbol_name, arg_string) == 0){
				int RVA = *(unsigned int*)header_address;
				if (opcode < 256)	
					return RVA - ((char*)instruction_database->instruction_translation_start[opcode] - instruction_database->text_section_address);
				else if(opcode == 256) 
					return RVA - ((char*) instruction_database->ppu_event_test - instruction_database->text_section_address);
			}	
			
		}	
	}
	return -1;
}
