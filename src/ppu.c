#include "execution_context.h"
#include "ppu.h"
#include "log_messages.h"
#include <string.h>
#include <unistd.h>

#define SHOW_TILE_DEBUG_GRIDS 0

static unsigned long long colors[] = {
	0x7C7C7C, 0x0000FC, 0x0000BC, 0x4428BC, 
	0x940084, 0xA80020, 0xA81000, 0x881400, 
	0x503000, 0x007800, 0x006800, 0x005800, 
	0x004058, 0x000000, 0x000000, 0x000000, 
	0xBCBCBC, 0x0078F8, 0x0058F8, 0x6844FC, 
	0xD800CC, 0xE40058, 0xF83800, 0xE45C10, 
	0xAC7C00, 0x00B800, 0x00A800, 0x00A844, 
	0x008888, 0x000000, 0x000000, 0x000000, 
	0xF8F8F8, 0x3CBCFC, 0x6888FC, 0x9878F8, 
	0xF878F8, 0xF85898, 0xF87858, 0xFCA044, 
	0xF8B800, 0xB8F818, 0x58D854, 0x58F898, 
	0x00E8D8, 0x787878, 0x000000, 0x000000, 
	0xFCFCFC, 0xA4E4FC, 0xB8B8F8, 0xD8B8F8, 
	0xF8B8F8, 0xF8A4C0, 0xF0D0B0, 0xFCE0A8, 
	0xF8D878, 0xD8F878, 0xB8F8B8, 0xB8F8D8, 
	0x00FCFC, 0xF8D8F8, 0x000000, 0x000000
};

PPU *create_ppu(FILE_HANDLE *file_handle) {
	PPU *ppu = malloc(sizeof(PPU));
	if(!ppu){
		ERROR_LOG("PPU memory could not be created\n");
		return NULL;
	}

	ppu->output_buffer = malloc(256*240*4);
	ppu->address_write_flag = 0;
	ppu->scroll_write_flag = 0;
	ppu->reg_ppustatus = 0;
	if(file_handle->size_of_chr_rom != 0x2000){
		ERROR_LOG("CHR ROM not found. Cannot initialize PPU\n");
		return NULL;	
	}
	memcpy(&ppu->VRAM[0], file_handle->start_of_chr_rom, file_handle->size_of_chr_rom);
	if(file_handle->nametable_mirroring == NAMETABLE_MIRROR_NONE)
		ppu->nametable_mirroring = UNMIRRORED;
	else if(file_handle->nametable_mirroring == NAMETABLE_MIRROR_DOUBLE)
		ppu->nametable_mirroring = SINGLE_SCREEN;
	else if (file_handle->nametable_mirroring == NAMETABLE_MIRROR_SINGLE){
		if(file_handle->file_headers->flags_6 & 0b1)		
			ppu->nametable_mirroring = VERTICAL;
		else
			ppu->nametable_mirroring = HORIZONTAL;
	}
	return ppu;	
}

void reset_ppu(PPU *ppu) {
	ppu->total_frames_rendered = 0;
	ppu->address_write_flag = 0;
	ppu->scroll_write_flag = 0;
	ppu->reg_ppustatus = 0 | STATUS_VERTICAL_BLANK;
	ppu->state = VERTICAL_BLANK;
	ppu->execution_context->cycles_to_ppu_event = CYCLES_PER_SCANLINE * scan_lines[VERTICAL_BLANK]; 
	int i = 0;
	for(i=0x2000; i < 0x3000; i++)
		ppu->VRAM[i] = 0;
}
void setPixel(PPU *ppu, int x, int y, BYTE color_id){
	unsigned long long color = colors[color_id];
	BYTE r = (color >> 16) & 0xff;
	BYTE g = (color >> 8) & 0xff;
	BYTE b = (color >> 0) & 0xff;
	ppu->output_buffer[(y * 256 + x)*4+0] = r;
	ppu->output_buffer[(y * 256 + x)*4+1] = g;
	ppu->output_buffer[(y * 256 + x)*4+2] = b;
	ppu->output_buffer[(y * 256 + x)*4+3] = color_id;
}

BYTE getPixel(PPU *ppu, int x, int y){
	return ppu->output_buffer[(y * 256 + x) * 4 + 3];
}


void coordinate_from_cycles(int ppuCC, int *x, int *y){
	*y = ppuCC / 341;
	*x = ppuCC % 341;
	//CPUcollisionCC = ((Y+21)*341+X)/3
}

void ppu_draw_screen(PPU *ppu, int fromX, int fromY, int toX, int toY) {
	WORD nametable[4];
	WORD pattern_table;
	if(ppu->reg_ppuctrl & 0b10000) 
		pattern_table = 0x1000;
	else
		pattern_table = 0x0000;
	BYTE backdrop_color = ppu->VRAM[0x3F00];
	BYTE pallete[4];	
	nametable[0] = 0x2000;
	switch(ppu->nametable_mirroring) {
		case VERTICAL:
			nametable[1] = 0x2400;
			nametable[2] = 0x2000;
			nametable[3] = 0x2400;
			break;
		case HORIZONTAL:
			nametable[1] = 0x2000;
			nametable[2] = 0x2800;
			nametable[3] = 0x2800;
			break;
		case SINGLE_SCREEN:
			nametable[1] = 0x2000;
			nametable[2] = 0x2000;
			nametable[3] = 0x2000;
			break;
		case UNMIRRORED:
			nametable[1] = 0x2400;
			nametable[2] = 0x2800;
			nametable[3] = 0x2C00;
			break;
		default:
			ERROR_LOG("Invalid nametable mirroring for PPU\n");
			return;
	}
	
	// We will assume now that we have to render from (0,0) to (256, 240)
	
	int startX = 0, endX = 256;
	int startY = fromY, endY = toY;
	int startClipX = fromX, endClipX = toX;
	int total_writes = 0;	
	int scrollX = ppu->reg_scroll_x;
	int scrollY = ppu->reg_scroll_y;
	if(ppu->nametable_selection & 0b1){
		scrollX+= 256;
	}
	if(ppu->nametable_selection & 0b10){
		scrollY+= 240;
	}
	startX += scrollX;
	startY += scrollY;
	endX += scrollX;
	endY += scrollY;

	endY++; // Adjustment for all calculation which are based on y < endY
	

	startClipX += scrollX;
	endClipX += scrollX;
	
	int tileStartX = startX / 8;
	int tileStartY = startY / 8;
	int tileEndX = endX / 8;
	int tileEndY = endY / 8;
	if(tileEndX * 8 != endX)
		tileEndX++;
	if(tileEndY * 8 != endY)
		tileEndY++;

	int tileXi, tileYi;
	for(tileYi = tileStartY; tileYi < tileEndY;tileYi++){
		int tileX, tileY, blockX, blockY, superblockX, superblockY;
		tileY = (tileYi + 30 + 60) % 30;
		blockY = tileY / 2;
		superblockY = blockY / 2;	
		for(tileXi = tileStartX; tileXi < tileEndX;tileXi++){
			int innerX;
			int innerY;			
			int nametable_id = (tileXi / 32) % 2 + (tileYi / 30)%2 * 2;
			tileX = (tileXi + 32 + 64) % 32;
			blockX = tileX / 2;
			superblockX = blockX / 2;

			WORD nametable_data = nametable[nametable_id];
			int attribute_byte = ppu->VRAM[nametable_data + 0x3C0 + superblockY * 8 + superblockX];
			int pallete_index = 0b11 & (attribute_byte >> ((2*(2 * (blockY % 2) + (blockX % 2)))));
			pallete[0] = backdrop_color;
			pallete[1] = ppu->VRAM[0x3F00 + pallete_index * 4 + 1];		
			pallete[2] = ppu->VRAM[0x3F00 + pallete_index * 4 + 2];		
			pallete[3] = ppu->VRAM[0x3F00 + pallete_index * 4 + 3];
			int tile_index = ppu->VRAM[nametable_data + tileY * 32 + tileX];
			int tile_data_plane1 = pattern_table + tile_index * 16;
			int tile_data_plane2 = pattern_table + tile_index * 16 + 8;

			for(innerY = 0; innerY < 8; innerY++){
				int realY = tileYi * 8 + innerY;
				BYTE plane1 = ppu->VRAM[tile_data_plane1 + innerY];
				BYTE plane2 = ppu->VRAM[tile_data_plane2 + innerY];
				if(realY < startY)
					continue;
				if(realY >= endY)
					break;
				for(innerX = 0; innerX < 8; innerX++){		
					int realX = tileXi * 8 + innerX;
					if(realX < startX)
						continue;
					if(realX >= endX)
						break;
					if(realY == startY && realX < startClipX)
						continue;
						
					if(realY == (endY - 1) && realX > endClipX)
						continue;

					int c_id = (plane1 >> (7-innerX) & 0b1) | ((plane2 >> (7-innerX) & 0b1) << 1);
					int x = (realX - scrollX + 256 + 256) % 256;
					int y = (realY - scrollY + 240 + 240) % 240;
					setPixel(ppu, x, y, pallete[c_id]);
					total_writes++;
				}
			}	

		}
	}



	/* Sprites rendering */

	int scanline;
	BYTE scanline_color[256];
	BYTE scanline_priority[256];
	int sprite_size = (ppu->reg_ppuctrl & 0b100000)?16:8;
	for(scanline = fromY; scanline <= toY; scanline++){
		int x;	
		for(x = 0; x < 256; x++)
			scanline_color[x] = backdrop_color;
		int sprite_index;
		for(sprite_index = 0; sprite_index < 64; sprite_index++){
			int y = ppu->OAM[sprite_index * 4] + 1;
			if(y >= 0xF0)
				continue;
			
			BYTE byte1 = ppu->OAM[sprite_index * 4 + 1];
			BYTE byte2 = ppu->OAM[sprite_index * 4 + 2];
			int x = ppu->OAM[sprite_index * 4 + 3];
			if(y+sprite_size <= scanline || scanline < y)
				continue;
			int line_from_sprite = scanline - y;
			if(byte2 & 0b10000000)
				line_from_sprite = sprite_size - line_from_sprite-1;
			WORD pattern_table;
			int tile_index;
			if(sprite_size == 8){
				pattern_table = (ppu->reg_ppuctrl & 0b1000)?0x1000:0x0000;
				tile_index = byte1;
			}
			else{
				tile_index = byte1 &0b11111110;
				pattern_table = (byte1 & 0b1)?0x1000:0x0000;
			}
			
			int tile_switcher = 0;
			if (line_from_sprite >= 8)
				tile_switcher = 0x8;

			BYTE plane1 = ppu->VRAM[pattern_table + 0x10 * tile_index + line_from_sprite + 0 + tile_switcher];
			BYTE plane2 = ppu->VRAM[pattern_table + 0x10 * tile_index + line_from_sprite + 8 + tile_switcher];
			int pallete_index = (byte2 & 0b11) + 4;
			BYTE pallete[4];
			pallete[0] = backdrop_color;
			pallete[1] = ppu->VRAM[0x3F00 + pallete_index * 4 + 1];		
			pallete[2] = ppu->VRAM[0x3F00 + pallete_index * 4 + 2];		
			pallete[3] = ppu->VRAM[0x3F00 + pallete_index * 4 + 3];
			int innerX;
			for(innerX = 0; innerX < 8; innerX++){
				if(innerX + x >= 256)
					continue;
				if (scanline_color[innerX + x] != backdrop_color)
					continue;
				int c_id;
				if(byte2 & 0b1000000)
					c_id = (plane1 >> (innerX) & 0b1) | ((plane2 >> (innerX) & 0b1) << 1);
				else
					c_id = (plane1 >> (7-innerX) & 0b1) | ((plane2 >> (7-innerX) & 0b1) << 1);
				scanline_color[innerX + x] = pallete[c_id];
				scanline_priority[innerX + x] = (byte2 >> 5) & 1;
			}	
		}
		for(x = 0; x < 256; x++){
			if(scanline == fromY && x < fromX)
				continue;
			if(scanline == toY && x > toX)
				continue;
			int tile_color = getPixel(ppu, x, scanline);
			if(scanline_color[x] != backdrop_color && (tile_color == backdrop_color || scanline_priority[x]==0))
				setPixel(ppu, x, scanline, scanline_color[x]);
		}
	}
	int x;
	
	INFO_LOG("Buffer render successful\n");
}


int calculate_sprite0_cycles(PPU *ppu) {
	return 30 * CYCLES_PER_SCANLINE;
}


WORD ppu_event_internal(EXECUTION_CONTEXT *execution_context, WORD next_address) {
	INFO_LOG("End of state %d triggered\n", execution_context->ppu->state);
	switch(execution_context->ppu->state) {
		case VERTICAL_BLANK:
			execution_context->ppu->state = PRE_RENDER_SCANLINE;
			execution_context->cycles_to_ppu_event += CYCLES_PER_SCANLINE * scan_lines[PRE_RENDER_SCANLINE];
			execution_context->ppu->reg_ppustatus &= NOT_STATUS_VERTICAL_BLANK;
			execution_context->ppu->reg_ppustatus &= NOT_STATUS_SPRITE0;
			break;
		case PRE_RENDER_SCANLINE:
			execution_context->ppu->state = SPRITE0_WAIT;
			execution_context->ppu->last_sprite0_cycle = calculate_sprite0_cycles(execution_context->ppu);
			execution_context->cycles_to_ppu_event += execution_context->ppu->last_sprite0_cycle;
			execution_context->ppu->plot_from_x = 0;
			execution_context->ppu->plot_from_y = 0;
			break;
		case SPRITE0_WAIT:
			execution_context->ppu->state = SPRITE0_DONE;
			execution_context->cycles_to_ppu_event += CYCLES_PER_SCANLINE * RENDERING_SCANLINES - execution_context->ppu->last_sprite0_cycle;
			execution_context->ppu->reg_ppustatus |= STATUS_SPRITE0; 
			break;
		case SPRITE0_DONE:
			execution_context->ppu->state = POST_RENDER_SCANLINE;
			execution_context->cycles_to_ppu_event +=CYCLES_PER_SCANLINE * scan_lines[POST_RENDER_SCANLINE];
			ppu_draw_screen(execution_context->ppu, execution_context->ppu->plot_from_x, execution_context->ppu->plot_from_y, 255, 239);
			update_display(execution_context->nes_display, execution_context->ppu->output_buffer);
			execution_context->ppu->total_frames_rendered ++;

			if(execution_context->ppu->total_frames_rendered != 1){
				struct timeval now;
				gettimeofday(&now, NULL);
				int elasped = ((now.tv_sec - execution_context->ppu->start_time.tv_sec) * 1000000) + (now.tv_usec - execution_context->ppu->start_time.tv_usec);
				int time_left = 1000000/60 - elasped;
				if(time_left > 0)
					usleep(time_left);
			}
			gettimeofday(&execution_context->ppu->start_time, NULL);

			break;
		case POST_RENDER_SCANLINE:
			execution_context->ppu->state = VERTICAL_BLANK;
			execution_context->cycles_to_ppu_event += CYCLES_PER_SCANLINE * scan_lines[VERTICAL_BLANK];
			execution_context->ppu->reg_ppustatus |= STATUS_VERTICAL_BLANK;
			execution_context->ppu->reg_scroll_x = 0;
			execution_context->ppu->reg_scroll_y = 0;
			execution_context->ppu->reg_ppuctrl &= 0b11111100;
			break;
		default:
			ERROR_LOG("Invalid state for ppu\n");
			exit(-1);
	}

	if((execution_context->ppu->reg_ppuctrl & execution_context->ppu->reg_ppustatus & 0b10000000) && !execution_context->interrupts_disabled){
		execution_context->nmi_flip_flop = 1;
	}

	INFO_LOG("Returning next_address from ppu_event = %04x\n", next_address);
	return next_address;
}

int destroy_ppu(PPU *ppu) {
	if(!ppu)
		return -1;
	/* Code to destoy a PPU instance */
	return 0;
}


void sync_ppu(PPU *ppu) {
	if(ppu->state == SPRITE0_WAIT || ppu->state == SPRITE0_DONE) {
		int x, y;
		int cycles_consumed = 0;
		if(ppu->state == SPRITE0_WAIT)
			cycles_consumed = ppu->last_sprite0_cycle - ppu->execution_context->cycles_to_ppu_event;
		else
			cycles_consumed = (240 * CYCLES_PER_SCANLINE - ppu->execution_context->cycles_to_ppu_event);
		coordinate_from_cycles(cycles_consumed, &x, &y);
		if( x > 255)
			x = 255;
		ppu_draw_screen(ppu, ppu->plot_from_x, ppu->plot_from_y, x, y);
		x++;
		if(x>255){
			x = 0;
			y++;
		}
		ppu->plot_from_x = x;
		ppu->plot_from_y = y;
	}
}


void write_ppuctrl(PPU* ppu, BYTE ctrl) {
	sync_ppu(ppu);
	ppu->reg_ppuctrl = ctrl;
	ppu->nametable_selection = ctrl & 0b11;
	if((ctrl & ppu->reg_ppustatus & 0b10000000) && !ppu->execution_context->interrupts_disabled){
		ppu->execution_context->nmi_flip_flop = 1;
	}
}
void write_ppumask(PPU *ppu, BYTE mask) {
	ppu->reg_ppumask = mask;
}
BYTE read_ppustatus(PPU *ppu) {
	ppu->scroll_write_flag = 0;
	ppu->address_write_flag = 0;
	BYTE return_val = ppu->reg_ppustatus;
	ppu->reg_ppustatus &= NOT_STATUS_VERTICAL_BLANK;
	return return_val;
}

void write_oamaddress(PPU *ppu, BYTE address) {
	ppu->reg_oamaddress = address;
}  

void write_oamdata(PPU *ppu, BYTE data) {
	ppu->OAM[ppu->reg_oamaddress] = data;
	ppu->reg_oamaddress++;
}
void write_oamdma(PPU *ppu, BYTE data) {
	void* address = translate_address_to_emulation_context(ppu->execution_context->file_handle, data * 0x100);	
	memcpy(ppu->OAM, address, 0x100);
	ppu->execution_context->cycles_to_ppu_event -= 513;
}

BYTE read_oamdata(PPU *ppu) {
	return ppu->OAM[ppu->reg_oamaddress];
}

void write_ppuscroll(PPU *ppu, BYTE scroll) {
	sync_ppu(ppu);
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
		ppu->nametable_selection=(address)/4;
		ppu->address_write_flag = 1;
	}else{
		ppu->reg_address_lower = address;
		ppu->address_write_flag = 0;
	}
	
}

void write_ppudata(PPU *ppu, BYTE data) {
	WORD address = (ppu->reg_address_upper << 8) | ppu->reg_address_lower;
	WORD actual_address = address;
	if(actual_address == 0x3F10 || actual_address == 0x3F14 || actual_address == 0x3F18 || actual_address == 0x3F1C)
		actual_address-= 0x10; 
	ppu->VRAM[actual_address] = data;
	if(ppu->reg_ppuctrl & 0b100)
		address += 32;
	else
		address++;
	address = address & 0xffff;
	ppu->reg_address_lower = address & 0xff;
	ppu->reg_address_upper = (address >> 8);
}


BYTE read_ppudata(PPU *ppu) {
	BYTE data = ppu->internal_vram_buffer;
	WORD address = (ppu->reg_address_upper << 8) | ppu->reg_address_lower;
	WORD actual_address = address;
	if(actual_address == 0x3F10 || actual_address == 0x3F14 || actual_address == 0x3F18 || actual_address == 0x3F1C)
		actual_address-= 0x10; 
	ppu->internal_vram_buffer = ppu->VRAM[actual_address];
	if(ppu->reg_ppuctrl & 0b100)
		address += 32;
	else
		address++;
	address = address & 0xffff;
	ppu->reg_address_lower = address & 0xff;
	ppu->reg_address_upper = (address >> 8);	
	return data;
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
		case OAMDMA:	write_oamdma(ppu, data); break;
		default:
			ERROR_LOG("Invalid write on PPU at address = %x\n", address);
			return;
	}
}



