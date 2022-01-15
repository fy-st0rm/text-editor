#ifndef _UTIL_H_
#define _UTIL_H_

#include "includes.h"


// SDL Error checker
static int sdl_check(int result)
{
	if (result < 0)
	{
		fprintf(stderr, "ERROR: %s\n", SDL_GetError());
		exit(1);
	}
	return result;
}

static void* sdl_check_ptr(void* result)
{
	if (!result)
	{
		fprintf(stderr, "PTR ERROR: %s\n", SDL_GetError());
		exit(1);
	}
	return result;
}

// SDL Text helpers
static SDL_Texture* create_texture(SDL_Renderer* renderer, TTF_Font* font, char* text)
{
	SDL_Color white = {255, 255, 255, 255};
	SDL_Surface* surface = sdl_check_ptr(TTF_RenderUTF8_Blended(font, text, white));
	SDL_Texture* texture = sdl_check_ptr(SDL_CreateTextureFromSurface(renderer, surface));

	SDL_FreeSurface(surface);
	return texture;
}

static void draw_text(SDL_Renderer* renderer, int x, int y, SDL_Texture* texture, SDL_Color color)
{
	SDL_Rect rect;
	SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);

	rect.x = x * rect.w;
	rect.y = y * rect.h;

	SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
	SDL_RenderCopy(renderer, texture, NULL, &rect);
}

#endif
