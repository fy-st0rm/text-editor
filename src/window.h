#ifndef _WINDOW_H_
#define _WINDOW_H_


#include "includes.h"
#include "globals.h"

// Editor modes
enum Modes
{
	NORMAL = 'N',
	INSERT = 'I',
	VISUAL = 'V',
	COMMAND= 'C'
};

typedef struct 
{
	SDL_Window* 	window;
	SDL_Renderer* 	renderer;
	int width, height;
	enum Modes mode;
} Window;

Window* window_new		(char* title, int w, int h);
void	window_clear	(Window* window, SDL_Color bg_color);
void	window_destroy	(Window* window);

#endif

