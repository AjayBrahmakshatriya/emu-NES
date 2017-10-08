#include "display.h"

static int event_thread(void *param) {
	NES_DISPLAY *nes_display = (NES_DISPLAY*) param;
	SDL_Event events;
	
	while(SDL_WaitEvent(&events)) {
        	if(events.type == SDL_QUIT)
            		exit(0);
		else if(events.type == SDL_KEYDOWN) {
			switch(events.key.keysym.sym){
				case SDLK_a:		nes_display->keypad1.A = 1; break;
				case SDLK_b:		nes_display->keypad1.B = 1; break;
				case SDLK_LEFT:		nes_display->keypad1.left = 1; break;
				case SDLK_RIGHT:	nes_display->keypad1.right = 1; break;
				case SDLK_UP:		nes_display->keypad1.up = 1; break;
				case SDLK_DOWN:		nes_display->keypad1.down = 1; break;
				case SDLK_RETURN:	nes_display->keypad1.start = 1; break;
				case SDLK_SPACE:	nes_display->keypad1.select = 1; break;
			}
		}
		else if(events.type == SDL_KEYUP) {
			switch(events.key.keysym.sym){
				case SDLK_a:		nes_display->keypad1.A = 0; break;
				case SDLK_b:		nes_display->keypad1.B = 0; break;
				case SDLK_LEFT:		nes_display->keypad1.left = 0; break;
				case SDLK_RIGHT:	nes_display->keypad1.right = 0; break;
				case SDLK_UP:		nes_display->keypad1.up = 0; break;
				case SDLK_DOWN:		nes_display->keypad1.down = 0; break;
				case SDLK_RETURN:	nes_display->keypad1.start = 0; break;
				case SDLK_SPACE:	nes_display->keypad1.select = 0; break;
			}
		}
    	}
	return 0;	
}


NES_DISPLAY* create_nes_display(void) {
	NES_DISPLAY *nes_display = malloc(sizeof(*nes_display));
	nes_display->window = SDL_CreateWindow("emu-NES", 100, 100, DISPLAY_WIDTH, DISPLAY_HEIGHT, SDL_WINDOW_SHOWN);
	nes_display->surface = SDL_GetWindowSurface(nes_display->window);
	nes_display->event_thread = SDL_CreateThread(event_thread, "EventThread", (void *)nes_display);

	nes_display->keypad1.A = 0;
	nes_display->keypad1.B = 0;
	nes_display->keypad1.left = 0;
	nes_display->keypad1.right = 0;
	nes_display->keypad1.up = 0;
	nes_display->keypad1.down = 0;
	nes_display->keypad1.select = 0;
	nes_display->keypad1.start = 0;

	nes_display->keypad2.A = 0;
	nes_display->keypad2.B = 0;
	nes_display->keypad2.left = 0;
	nes_display->keypad2.right = 0;
	nes_display->keypad2.up = 0;
	nes_display->keypad2.down = 0;
	nes_display->keypad2.select = 0;
	nes_display->keypad2.start = 0;
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
                4 * 8,
                DISPLAY_WIDTH * 4,
                0x0000FF,
                0x00FF00,
                0xFF0000,
                0);
	SDL_BlitSurface(image, NULL, nes_display->surface, NULL);
	SDL_UpdateWindowSurface(nes_display->window);
	return 0;
}
