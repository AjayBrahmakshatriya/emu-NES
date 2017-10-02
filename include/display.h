#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>

#define DISPLAY_HEIGHT 240
#define DISPLAY_WIDTH  256


typedef struct {
	SDL_Window *window;
	SDL_Surface *surface;
	SDL_Thread *event_thread;
}NES_DISPLAY;


NES_DISPLAY* create_nes_display(void);
int destroy_nes_display(NES_DISPLAY* nes_display);

int update_display(NES_DISPLAY* nes_display, unsigned char *output_buffer);


#endif
