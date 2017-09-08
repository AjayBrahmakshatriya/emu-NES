#ifndef INSTRUCTION_DATABASE_H
#define INSTRUCTION_DATABASE_H

#define FILE_PATH_SIZE 256

typedef struct {
	char file_name[FILE_PATH_SIZE];
	int fd;
	void* base_address;
	void* instruction_translation_start[256];
	char* symbol_table_address;
	int symbol_count;
	char* string_table_address;
	char* text_section_address;
	
} INSTRUCTION_DATABASE;



INSTRUCTION_DATABASE* create_database(char* filename);
int destroy_database(INSTRUCTION_DATABASE *instruction_database);

#endif
