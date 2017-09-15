#ifndef NES_DECODER_H
#define NES_DECODER_H
#include <stdlib.h>
#include "file_loader.h"

typedef enum {
	AM_A,
	AM_abs,
	AM_abs_X,
	AM_abs_Y,
	AM_hash,
	AM_impl,
	AM_ind,
	AM_X_ind,
	AM_ind_Y,
	AM_rel,
	AM_zpg,
	AM_zpg_X,
	AM_zpg_Y,
}NES_ADDRESSING_MODE;
static const int AM_sizes[] = {
	[AM_A] = 1,
	[AM_abs] = 3,
	[AM_abs_X] = 3,
	[AM_abs_Y] = 3,
	[AM_hash] = 2,
	[AM_impl] = 1,
	[AM_ind] = 3,
	[AM_X_ind] = 2,
	[AM_ind_Y] = 2,
	[AM_rel] = 2,
	[AM_zpg] = 2,
	[AM_zpg_X] = 2,
	[AM_zpg_Y] = 2
};
static const char * const AM_names[] = {
	[AM_A] = "A",
	[AM_abs] = "abs",
	[AM_abs_X] = "abs, X",
	[AM_abs_Y] = "abs, Y",
	[AM_hash] = "#",
	[AM_impl] = "impl",
	[AM_ind] = "ind",
	[AM_X_ind] = "ind, X",
	[AM_ind_Y] = "Y, ind",
	[AM_rel] = "rel",
	[AM_zpg] = "zpg",
	[AM_zpg_X] = "zpg, X",
	[AM_zpg_Y] = "zpg, Y"
};
typedef struct {
	const char* name;
	NES_ADDRESSING_MODE addressing_mode;	
} NES_INSTRUCTION;

extern NES_INSTRUCTION nes_instructions[256];
extern int *BB_END_OPCODES;
extern int BB_END_OPCODES_COUNT;

const NES_INSTRUCTION* decode_address(void* address, int* opcode, int* arguments, int* size);
void decode_and_print_from(FILE_HANDLE *file_handle, void *address, size_t to_print);
int is_bb_end_opcode(int opcode);
#endif


