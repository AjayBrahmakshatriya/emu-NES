#ifndef EXECUTION_CONTEXT_H
#define EXECUTION_CONTEXT_H

#include "file_loader.h"
#include "execution_area.h"
#include "instruction_database.h"

typedef struct {
	void (*basic_block_end)(void);
}EMULATION_VECTOR;
typedef struct {
	unsigned long long  A; // Mapped to %r10 	// Offset 0x00
	unsigned long long  X; // Mapped to %r11	// Offset 0x08
	unsigned long long  Y; // Mapped to %r12	// Offset 0x10
	unsigned long long  S; // Mapped to %r13 	// Offset 0x18
	unsigned long long SR; // Mapped to %r14	// Offset 0x20
	EMULATION_VECTOR *emulation_vector; // Mapped to %r15	// Offset 0x28
}REGISTERS;

typedef struct {
	REGISTERS registers;	// Offset 0	
	FILE_HANDLE *file_handle;
	EXECUTION_AREA *execution_area;
	INSTRUCTION_DATABASE *instruction_database;
}EXECUTION_CONTEXT;





void initialize_execution_context(EXECUTION_CONTEXT *execution_context, FILE_HANDLE *file_handle, EXECUTION_AREA *execution_area, INSTRUCTION_DATABASE *instruction_database);
void* get_execution_address(EXECUTION_CONTEXT *execution_context, unsigned long long address);



// Functions onward here are implemented in assembly

void start_execution(EXECUTION_CONTEXT *execution_context);

#endif
