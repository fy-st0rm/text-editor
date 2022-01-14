#ifndef _CMD_LINE_H_
#define _CMD_LINE_H_

#include "includes.h"
#include "window.h"
#include "editor.h"

#define MAX_INPUT 100

typedef struct
{
	Window* window;

	// Textures
	SDL_Texture* texture;

	char* input;
	char* prompt;

	bool active;

	int cur_x, cur_y, cur_w, cur_h;
	int scroll_x;
} Cmd_line;

Cmd_line* 	cmd_line_new			(Window* window, TTF_Font* font);
void		cmd_line_destroy		(Cmd_line* cmd_line);
void		cmd_line_resize			(Cmd_line* cmd_line);

// Command line event functions
void 		cmd_line_insert			(Cmd_line* cmd_line, char ch);
void		cmd_line_backspace  	(Cmd_line* cmd_line);
void		cmd_line_parse			(Cmd_line* cmd_line, Editor** buffers, int* curr_buffer);

// Prompt
void		cmd_line_set_prompt		(Cmd_line* cmd_line, char* new_prompt);
void		cmd_line_clear_prompt	(Cmd_line* cmd_line);

// Input
void		cmd_line_clear_input	(Cmd_line* cmd_line);

// Cursor
void		cmd_line_cur_left		(Cmd_line* cmd_line);
void		cmd_line_cur_right		(Cmd_line* cmd_line);

// Command line render functions
void		cmd_line_render			(Cmd_line* cmd_line, TTF_Font* font, Colors* colors_rgb);

#endif
