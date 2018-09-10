#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>

#define DISPLAY_HEIGHT 240
#define DISPLAY_WIDTH  256

#include "types.h"

typedef struct {
	BYTE A;		// A
	BYTE B;		// B
	BYTE left;	// <
	BYTE right;	// >
	BYTE up;	// ^
	BYTE down;	// v
	BYTE select;	// space
	BYTE start;	// enter
} KEYPAD;


typedef struct {
	SDL_Window *window;
	SDL_Surface *surface;
	//SDL_Thread *event_thread;
	int (*keyboard_event)(void *);
	KEYPAD keypad1;
	KEYPAD keypad2;
}NES_DISPLAY;


NES_DISPLAY* create_nes_display(void);
int destroy_nes_display(NES_DISPLAY* nes_display);

int update_display(NES_DISPLAY* nes_display, unsigned char *output_buffer);


#endif
