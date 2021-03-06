#ifndef EXECUTION_CONTEXT_H
#define EXECUTION_CONTEXT_H

#include "file_loader.h"
#include "execution_area.h"
#include "instruction_database.h"
#include "ppu.h"
#include "display.h"

typedef struct {
	void (*basic_block_end)(void);					// Offset 0x00
	void (*terminate_execution)(void);				// Offset 0x08
	void (*read_non_ram_address)(void);				// Offset 0x10
	void (*write_non_ram_address)(void);				// Offset 0x18
	void (*ppu_event)(void);					// Offset 0x20
	void (*nmi_trigger)(void);					// Offset 0x28
}EMULATION_VECTOR;

typedef struct {
	unsigned long long  A;			// Mapped to %r10	// Offset 0x00
	unsigned long long  X;			// Mapped to %r11	// Offset 0x08
	unsigned long long  Y;			// Mapped to %r12	// Offset 0x10
	unsigned long long  S;			// Mapped to %r13 	// Offset 0x18
	unsigned long long SR;			// Mapped to %r14	// Offset 0x20
	EMULATION_VECTOR *emulation_vector;	// Mapped to %r15	// Offset 0x28
	void *base_address;			// Mapped to %rbx	// Offset 0x28
	unsigned long long FreeSlot1;					// Offset 0x38
	unsigned long long FreeSlot2;					// Offset 0x40
	unsigned long long FreeSlot3;					// Offset 0x48
	unsigned long long FreeSlot4;					// Offset 0x50
	unsigned long long FreeSlot5;					// Offset 0x58	
}REGISTERS;

typedef enum {
	controller_A = 0,
	controller_B = 1,
	controller_select = 2,
	controller_start = 3,
	controller_up = 4,
	controller_down = 5,
	controller_left = 6,
	controller_right = 7,
	controller_loop = 8,
}CONTROLLER_CYCLE;

typedef struct {
	BYTE input;
	CONTROLLER_CYCLE cycle1;
	CONTROLLER_CYCLE cycle2;
}CONTROLLER_STATE;

typedef struct _EXECUTION_CONTEXT {
	REGISTERS registers;						// Offset 0x00	
	FILE_HANDLE *file_handle; 					// Offset 0x60
	EXECUTION_AREA *execution_area; 				// Offset 0x68 
	INSTRUCTION_DATABASE *instruction_database; 			// Offset 0x70
	long long cycles_to_ppu_event; 					// Offset 0x78
	unsigned long long nmi_flip_flop;				// Offset 0x80
	unsigned long long interrupts_disabled;				// Offset 0x88
	EMULATION_VECTOR emulation_vector;
	PPU *ppu;
	NES_DISPLAY *nes_display;
	CONTROLLER_STATE controller_state;
}EXECUTION_CONTEXT;





void initialize_execution_context(EXECUTION_CONTEXT *execution_context, FILE_HANDLE *file_handle, EXECUTION_AREA *execution_area, INSTRUCTION_DATABASE *instruction_databasei, PPU *ppu, NES_DISPLAY *nes_display);
void* get_execution_address(EXECUTION_CONTEXT *execution_context, unsigned long long address);


unsigned char read_non_ram_address_internal(EXECUTION_CONTEXT *execution_context, unsigned short address);
void write_non_ram_address_internal(EXECUTION_CONTEXT *execution_context, unsigned short address, unsigned char value);

// Functions onward here are implemented in assembly

void start_execution(EXECUTION_CONTEXT *execution_context);
void basic_block_end(void);
void read_non_ram_address(void);
void write_non_ram_address(void);
void ppu_event(void);
void nmi_trigger(void);
#endif
