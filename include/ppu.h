#ifndef PPU_H
#define PPU_H

#define PPUCTRL		0x2000
#define PPUMASK		0x2001
#define PPUSTATUS	0x2002

#define OAMADDR		0x2003
#define OAMDATA		0x2004

#define PPUSCROLL	0x2005

#define PPUADDR		0x2006
#define	PPUDATA		0x2007

#define	OAMDMA		0x4014
#define __read__
#define __write__



typedef unsigned char BYTE;
typedef unsigned short WORD;


typedef enum {
	VERTICAL_BLANK = 0,
	PRE_RENDER_SCANLINE = 1,
	SPRITE0_WAIT = 2,
	SPRITE0_DONE = 3,
	POST_RENDER_SCANLINE = 4
}PPU_STATES;


static int scan_lines[] = {20, 1, 0, 0, 1};


#define RENDERING_SCANLINES 240

#define CYCLES_PER_SCANLINE 341

#define STATUS_VERTICAL_BLANK		0b10000000
#define STATUS_SPRITE0			0b01000000
#define STATUS_SPRITE_OVERFLOW		0b00100000

#define NOT_STATUS_VERTICAL_BLANK	0b01111111
#define NOT_STATUS_SPRITE0		0b10111111
#define NOT_STATUS_SPRITE_OVERFLOW	0b11011111


typedef enum {
	VERTICAL = 0,
	HORIZONTAL = 1,
	SINGLE_SCREEN = 2,
	UNMIRRORED = 3,
}NAMETABLE_MIRRORING;

struct _EXECUTION_CONTEXT;
typedef struct {
	unsigned char *output_buffer;

	BYTE reg_ppuctrl;
	BYTE reg_ppumask;
	BYTE reg_ppustatus;

	BYTE reg_oamaddress;

	BYTE reg_scroll_x;
	BYTE reg_scroll_y;
	BYTE scroll_write_flag;

	BYTE reg_address_upper;
	BYTE reg_address_lower;
	BYTE address_write_flag;

	BYTE VRAM[0x4000];
	BYTE OAM[0xFF];
	struct _EXECUTION_CONTEXT *execution_context;
	PPU_STATES state;
	int last_sprite0_cycle;
	NAMETABLE_MIRRORING nametable_mirroring;
	BYTE internal_vram_buffer;
}PPU;


PPU* create_ppu(FILE_HANDLE *file_handle);
int destroy_ppu(PPU* ppu);

BYTE ppu_read(PPU* ppu, WORD address);
void ppu_write(PPU* ppu, WORD address, BYTE data);

void reset_ppu(PPU *ppu);

WORD ppu_event_internal(struct _EXECUTION_CONTEXT *execution_context, WORD next_address);
#endif
