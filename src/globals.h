#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "includes.h"
#include "util.h"

// Datatypes
static const char c_data_types[] = "int float char short long double signed unsigned size_t uint8_t uint16_t uint32_t";

// Replies
static const char replies[9][100] = {
	{"S0: File saved sucessfully."},
	{"E1: No file name."},
	{"E2: Failed to open file."},
	{"S1: File read sucessfully."},
	{"E3: File is not saved. (add ! to exit without saving)"},
	{"E4: Buffer is not modifiable."},
	{"S2: Created new buffer."},
	{"S3: Sucessfully destroyed the buffer."},
	{"E5: Invalid command."}
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
	SDL_Color cur_line_fg;
	SDL_Color bar_fg;

	// Mode colors
	SDL_Color normal_md;
	SDL_Color insert_md;
	SDL_Color visual_md;
	SDL_Color command_md;

	// Cursor
	SDL_Color cursor;
	SDL_Color selection;
} Colors;

// Settings
typedef struct
{
	// Fonts
	char family1[256];
	char family2[256];
	int font_size_1;
	int font_size_2;

	bool auto_indent;
} Settings;

#endif
