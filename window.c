#include "window.h"

Window* window_new(char* title, int w, int h)
{
	Window* window = malloc(sizeof(Window));

	window->width = w;
	window->height = h;
	window->mode = NORMAL;

	window->window = sdl_check_ptr(SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_RESIZABLE));
	window->renderer = sdl_check_ptr(SDL_CreateRenderer(window->window, -1, SDL_RENDERER_ACCELERATED));

	return window;
}

void window_clear(Window* window, SDL_Color bg_color)
{
	SDL_RenderPresent(window->renderer);
	SDL_SetRenderDrawColor(window->renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
	SDL_RenderClear(window->renderer);
}

void window_destroy(Window* window)
{
	SDL_DestroyWindow(window->window);
	SDL_DestroyRenderer(window->renderer);
	free(window);
}
