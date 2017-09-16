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
	char* text_relocation_address;
	int text_relocation_count;
	
} INSTRUCTION_DATABASE;



INSTRUCTION_DATABASE* create_database(char* filename);
int destroy_database(INSTRUCTION_DATABASE *instruction_database);
void *find_instruction_start(INSTRUCTION_DATABASE *instruction_database, int opcode);
long long find_arg_location(INSTRUCTION_DATABASE *instruction_database, int opcode, int arg);

#endif
