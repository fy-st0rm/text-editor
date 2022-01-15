#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "includes.h"
#include "util.h"

// Editor modes
enum Modes
{
	NORMAL = 'N',
	INSERT = 'I',
	VISUAL = 'V',
	COMMAND= 'C'
};

// Replies
static const char replies[6][100] = {
	{"S0: File saved sucessfully."},
	{"E1: No file name."},
	{"E2: Failed to open file."},
	{"S1: File read sucessfully."},
	{"E3: File is not saved. (add ! to exit without saving)"},
	{"E4: Buffer is not modifiable."}
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


#endif
