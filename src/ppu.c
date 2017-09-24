#include "execution_context.h"
#include "ppu.h"
#include "log_messages.h"




PPU *create_ppu(void) {
	PPU *ppu = malloc(sizeof(PPU));
	if(!ppu){
		ERROR_LOG("PPU memory could not be created\n");
		return NULL;
	}

	ppu->output_buffer = malloc(256 * 254 * 3);
	ppu->address_write_flag = 0;
	ppu->scroll_write_flag = 0;

	return ppu;
}
int destroy_ppu(PPU *ppu) {
	if(!ppu)
		return -1;
	/* Code to destoy a PPU instance */

	return 0;
}



void write_ppuctrl(PPU* ppu, BYTE ctrl) {
	ppu->reg_ppuctrl = ctrl;
}
void write_ppumask(PPU *ppu, BYTE mask) {
	ppu->reg_ppumask = mask;
}
BYTE read_ppustatus(PPU *ppu) {
	ppu->scroll_write_flag = 0;
	ppu->address_write_flag = 0;
	return ppu->reg_ppustatus;
}

void write_oamaddress(PPU *ppu, BYTE address) {
	ppu->reg_oamaddress = address;
}  

void write_oamdata(PPU *ppu, BYTE data) {
	ppu->OAM[ppu->reg_oamaddress] = data;
	ppu->reg_oamaddress++;
}

BYTE read_oamdata(PPU *ppu) {
	return ppu->OAM[ppu->reg_oamaddress];
}

void write_ppuscroll(PPU *ppu, BYTE scroll) {
	if(ppu->scroll_write_flag == 0) {
		ppu->reg_scroll_x = scroll;
		ppu->scroll_write_flag = 1;
	}else{
		ppu->reg_scroll_y = scroll;
		ppu->scroll_write_flag = 0;
	}
}

void write_ppuaddress(PPU *ppu, BYTE address) {
	if(ppu->address_write_flag == 0) {
		ppu->reg_address_upper = address;
		ppu->address_write_flag = 1;
	}else{
		ppu->reg_address_lower = address;
		ppu->address_write_flag = 0;
	}

}

void write_ppudata(PPU *ppu, BYTE data) {
	ppu->VRAM[(ppu->reg_address_upper << 8) | ppu->reg_address_lower] = data;
	ppu->reg_address_lower++;
	if(ppu->reg_address_lower == 0x00)
		ppu->reg_address_upper++;
}


BYTE read_ppudata(PPU *ppu) {
	return ppu->VRAM[(ppu->reg_address_upper << 8) | ppu->reg_address_lower];
}


BYTE ppu_read(PPU *ppu, WORD address) {
	switch(address) {
		case PPUSTATUS: return read_ppustatus(ppu);
		case OAMDATA: 	return read_oamdata(ppu);
		case PPUDATA: 	return read_ppudata(ppu);
		default:
			ERROR_LOG("Invalid read on PPU at address = %x\n", address);			    return -1;
	}
}

void ppu_write(PPU *ppu, WORD address, BYTE data) {
	switch(address) {
		case PPUCTRL: 	write_ppuctrl(ppu, data); break;
		case PPUMASK:	write_ppumask(ppu, data); break;
		case OAMADDR:	write_oamaddress(ppu, data); break;
		case OAMDATA:	write_oamdata(ppu, data); break;
		case PPUSCROLL:	write_ppuscroll(ppu, data); break;
		case PPUADDR:	write_ppuaddress(ppu, data); break;
		case PPUDATA:	write_ppudata(ppu, data); break;
		default:
			ERROR_LOG("Invalid write on PPU at address = %x\n", address);
			return;
	}
}



