#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "util.h"

typedef struct 
{
	SDL_Window* 	window;
	SDL_Renderer* 	renderer;
	int width, height;
} Window;

Window* window_new		(char* title, int w, int h);
void	window_clear	(Window* window, SDL_Color bg_color);
void	window_destroy	(Window* window);

#endif

