#include "execution_context.h"
#include "ppu.h"
#include "log_messages.h"
#include <string.h>

PPU *create_ppu(FILE_HANDLE *file_handle) {
	PPU *ppu = malloc(sizeof(PPU));
	if(!ppu){
		ERROR_LOG("PPU memory could not be created\n");
		return NULL;
	}

	ppu->output_buffer = malloc(512*480);
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
	ppu->address_write_flag = 0;
	ppu->scroll_write_flag = 0;
	ppu->reg_ppustatus = 0 | STATUS_VERTICAL_BLANK;
	ppu->state = VERTICAL_BLANK;
	ppu->execution_context->cycles_to_ppu_event = CYCLES_PER_SCANLINE * scan_lines[VERTICAL_BLANK]; 
	int i = 0;
	for(i=0x2000; i < 0x3000; i++)
		ppu->VRAM[i] = 0;
}

void setPixel(PPU *ppu, int x, int y, BYTE pixel) {
	ppu->output_buffer[y * 512 + x] = pixel;
}


void render_nametable(PPU *ppu, WORD nametable_data, int startX, int startY){
	WORD pattern_table;
	if(ppu->reg_ppuctrl & 0b10000) 
		pattern_table = 0x1000;
	else
		pattern_table = 0x0000;
	BYTE backdrop_color = ppu->VRAM[0x3F00];
	BYTE pallete[4];
	int tileX, tileY;
	for(tileY = 0; tileY < 30; tileY++){
		for (tileX = 0; tileX < 32; tileX++){
			int blockX = tileX / 2;
			int blockY = tileY / 2;
			int superblockX = blockX / 2;
			int superblockY = blockY / 2;
			int attribute_byte = ppu->VRAM[nametable_data + 0x3C0 + superblockY * 8 + superblockX];
			int pallete_index = 0b11 & (attribute_byte >> (6-(2*(2 * (blockY % 2) + (blockX % 2)))));
			pallete[0] = backdrop_color;
			pallete[1] = ppu->VRAM[0x3F00 + pallete_index * 4 + 1];		
			pallete[2] = ppu->VRAM[0x3F00 + pallete_index * 4 + 2];		
			pallete[3] = ppu->VRAM[0x3F00 + pallete_index * 4 + 3];
			int tile_index = ppu->VRAM[nametable_data + tileY * 32 + tileX];
			int tile_data_plane1 = pattern_table + tile_index * 16;
			int tile_data_plane2 = pattern_table + tile_index * 16 + 8;
			int x, y;

			for(y = 0; y < 8; y++){
				BYTE plane1 = ppu->VRAM[tile_data_plane1 + y];
				BYTE plane2 = ppu->VRAM[tile_data_plane2 + y];
				for (x = 0; x < 8; x++) {
					if(x == 0 && y == 0){
						setPixel(ppu, startX + tileX * 8 + x, startY + tileY * 8 + y, 0xf);
						continue;
					}
						
					int c_id = (plane1 >> (7-x) & 0b1) | ((plane2 >> (7-x) & 0b1) << 1);
					setPixel(ppu, startX + tileX * 8 + x, startY + tileY * 8 + y, pallete[c_id]);
				}
			}
		}
	}
}

void ppu_draw_screen(PPU *ppu) {
	//FILE *dump = fopen("dump.ram", "rb");
	//fread(&(ppu->VRAM[0x2000]), 0x400, 1, dump);
	//fclose(dump);

	WORD nametable0, nametable1, nametable2, nametable3;
	nametable0 = 0x2000;
	switch(ppu->nametable_mirroring) {
		case VERTICAL:
			nametable1 = 0x2400;
			nametable2 = 0x2000;
			nametable3 = 0x2400;
			break;
		case HORIZONTAL:
			nametable1 = 0x2000;
			nametable2 = 0x2800;
			nametable3 = 0x2800;
			break;
		case SINGLE_SCREEN:
			nametable1 = 0x2000;
			nametable2 = 0x2000;
			nametable3 = 0x2000;
			break;
		case UNMIRRORED:
			nametable1 = 0x2400;
			nametable2 = 0x2800;
			nametable3 = 0x2C00;
			break;
		default:
			ERROR_LOG("Invalid nametable mirroring for PPU\n");
			return;
	}
	
	render_nametable(ppu, nametable0, 0, 0);
	render_nametable(ppu, nametable1, 256, 0);
	render_nametable(ppu, nametable2, 0, 240);
	render_nametable(ppu, nametable3, 256, 240);


	FILE *output_file = fopen("temp_file.txt", "wb");
	fwrite(ppu->output_buffer, 480 * 512, 1, output_file);
	fclose(output_file); 
	INFO_LOG("Buffer render successful\n");
}


int calculate_sprite0_cycles(PPU *ppu) {
	return 20 * CYCLES_PER_SCANLINE;
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
			break;
		case SPRITE0_WAIT:
			execution_context->ppu->state = SPRITE0_DONE;
			execution_context->cycles_to_ppu_event += CYCLES_PER_SCANLINE * RENDERING_SCANLINES - execution_context->ppu->last_sprite0_cycle;
			execution_context->ppu->reg_ppustatus |= STATUS_SPRITE0; 
			break;
		case SPRITE0_DONE:
			execution_context->ppu->state = POST_RENDER_SCANLINE;
			execution_context->cycles_to_ppu_event +=CYCLES_PER_SCANLINE * scan_lines[POST_RENDER_SCANLINE];
			ppu_draw_screen(execution_context->ppu);
			break;
		case POST_RENDER_SCANLINE:
			execution_context->ppu->state = VERTICAL_BLANK;
			execution_context->cycles_to_ppu_event += CYCLES_PER_SCANLINE * scan_lines[VERTICAL_BLANK];
			execution_context->ppu->reg_ppustatus |= STATUS_VERTICAL_BLANK;
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



void write_ppuctrl(PPU* ppu, BYTE ctrl) {
	ppu->reg_ppuctrl = ctrl;
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



