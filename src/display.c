#include "display.h"


NES_DISPLAY* create_nes_display(void) {
	NES_DISPLAY *nes_display = malloc(sizeof(*nes_display));
	nes_display->window = SDL_CreateWindow("emu-NES", 100, 100, DISPLAY_WIDTH, DISPLAY_HEIGHT, SDL_WINDOW_SHOWN);
	nes_display->surface = SDL_GetWindowSurface(nes_display->window);
	return nes_display;
}


int destroy_nes_display(NES_DISPLAY *nes_display) {
	if(nes_display == NULL)
		return -1;
	/* Code for destroying display */
	return 0;
}


int update_display(NES_DISPLAY *nes_display, unsigned char *output_buffer) {
	SDL_Surface *image = SDL_CreateRGBSurfaceFrom((void*)output_buffer,
                DISPLAY_WIDTH,
                DISPLAY_HEIGHT,
                3 * 8,
                DISPLAY_WIDTH * 3,
                0x0000FF,
                0x00FF00,
                0xFF0000,
                0);
	SDL_BlitSurface(image, NULL, nes_display->surface, NULL);
	SDL_UpdateWindowSurface(nes_display->window);
	SDL_Event events;
	while(SDL_PollEvent(&events)) {
        	if(events.type == SDL_QUIT)
            		exit(0);
    	}
	return 0;
}
