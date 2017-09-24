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
}PPU;


PPU* create_ppu(void);
int destroy_ppu(PPU* ppu);

BYTE ppu_read(PPU* ppu, WORD address);
void ppu_write(PPU* ppu, WORD address, BYTE data);


#endif
