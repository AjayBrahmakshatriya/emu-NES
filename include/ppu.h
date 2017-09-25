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
	RENDERING = 2,
	POST_RENDER_SCANLINE = 3
}PPU_STATES;


static int scan_lines[] = {20, 1, 240, 1};
#define CYCLES_PER_SCANLINE 341

#define STATUS_VERTICAL_BLANK		0b10000000
#define STATUS_SPRITE0			0b01000000
#define STATUS_SPRITE_OVERFLOW		0b00100000

#define NOT_STATUS_VERTICAL_BLANK	0b01111111
#define NOT_STATUS_SPRITE0		0b10111111
#define NOT_STATUS_SPRITE_OVERFLOW	0b11011111

struct _EXECUTION_CONTEXT;
typedef struct _EXECUTION_CONTEXT EXECUTION_CONTEXT;
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

	BYTE VRAM[0x2000];
	BYTE OAM[0xFF];
	EXECUTION_CONTEXT *execution_context;
	PPU_STATES state;	
}PPU;


PPU* create_ppu(void);
int destroy_ppu(PPU* ppu);

BYTE ppu_read(PPU* ppu, WORD address);
void ppu_write(PPU* ppu, WORD address, BYTE data);

void reset_ppu(PPU *ppu);

void ppu_event_internal(EXECUTION_CONTEXT *execution_context, WORD next_address);
#endif
