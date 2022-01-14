#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "includes.h"

// Editor modes
enum Modes
{
	NORMAL = 'N',
	INSERT = 'I',
	VISUAL = 'V',
	COMMAND= 'C'
};

// Colors
typedef struct 
{
	// Background
	SDL_Color editor_bg;
	SDL_Color line_bg;
	SDL_Color bar_bg;

	// Foreground
	SDL_Color editor_fg;
	SDL_Color line_fg;
	SDL_Color sel_line_fg;
	SDL_Color bar_fg;

	// Mode colors
	SDL_Color normal_md;
	SDL_Color insert_md;
	SDL_Color visual_md;
	SDL_Color command_md;

	// Cursor
	SDL_Color cursor;
} Colors;

static Colors* colors_rgb;

static void init_colors()
{
	colors_rgb = malloc(sizeof(Colors));

	// Background
	SDL_Color editor_bg = {  40,  40,  40 };
	SDL_Color line_bg   = {  40,  40,  40 };
	SDL_Color bar_bg    = {  60,  56,  54 };

	// Foreground
	SDL_Color editor_fg   = { 251, 241, 199 };
	SDL_Color line_fg  	  = { 251, 241, 199 }; 
	SDL_Color sel_line_fg = { 254, 128,  25 };
	SDL_Color bar_fg      = { 251, 241, 199 };	

	// Mode colors
	SDL_Color normal_md  = { 184, 187,  38 };
	SDL_Color insert_md  = { 131, 165, 152 };
	SDL_Color visual_md  = { 254, 128,  25 };
	SDL_Color command_md = { 184, 187,  38 };
	
	// Cursor
	SDL_Color cursor = { 235, 219, 178 };

	colors_rgb->editor_bg = editor_bg;
	colors_rgb->editor_fg = editor_fg;

   	colors_rgb->line_bg	= line_bg;
	colors_rgb->line_fg = line_fg;
	colors_rgb->sel_line_fg = sel_line_fg;

	colors_rgb->bar_bg = bar_bg;
	colors_rgb->bar_fg = bar_fg;
	
	colors_rgb->normal_md = normal_md;
	colors_rgb->insert_md = insert_md;
	colors_rgb->visual_md = visual_md;
	colors_rgb->command_md = command_md;

	colors_rgb->cursor = cursor;
}

static void free_colors()
{
	free(colors_rgb);
}

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
