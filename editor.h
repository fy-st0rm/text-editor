#ifndef _EDITOR_H_
#define _EDITOR_H_

#include "globals.h"
#include "window.h"

#define TAB_SIZE 	4

typedef struct
{
	char file_name[256];
	char* text_buffer;
	int buffer_len;
	Window* window;

	// Cursor
	int cur_x, cur_y, cur_rend_x, cur_rend_y, cur_w, cur_h;
	
	// Scroll
	int scroll_x, scroll_y;

	SDL_Texture* editor_texture;
	SDL_Texture* line_texture;
	SDL_Texture* bar_texture;
} Editor;

Editor* 		editor_new			(Window* window, TTF_Font* font, char* file_name);
void			editor_destroy		(Editor* editor);
void 			editor_resize		(Editor* editor);

// Editor file handling
int				editor_read_file	(Editor* editor, char* file_name);
int				editor_write_file	(Editor* editor, char* file_name);

// Editor buffer management
int				editor_get_cur_pos	(Editor* editor, int x, int y);
int				editor_get_line_no	(Editor* editor);
void			editor_get_line		(Editor* editor, int line_no, char* out);
int				editor_line_len		(Editor* editor, int line_no);
void			editor_insert		(Editor* editor, char chr);
void			editor_backspace	(Editor* editor);

// Editor cursor
void			editor_cur_left		(Editor* editor);
void			editor_cur_right	(Editor* editor);
void			editor_cur_up		(Editor* editor);
void			editor_cur_down		(Editor* editor);

// Editor scrolls
void			editor_scroll_left	(Editor* editor);
void			editor_scroll_right (Editor* editor);
void			editor_scroll_up	(Editor* editor);
void			editor_scroll_down	(Editor* editor);

// Editor rendering
void			editor_render_buffer(Editor* editor, int start, int end, TTF_Font* font, Colors* color_rgb);
void 			editor_render_line	(Editor* editor, int start, int end, TTF_Font* font, Colors* color_rgb);
void			editor_render_bar	(Editor* editor, TTF_Font* font, Colors* color_rgb);
void			editor_render		(Editor* editor, Window* window, TTF_Font* font, Colors* colors_rgb);
void			editor_gen_texture	(Editor* editor, SDL_Renderer* renderer, TTF_Font* font);
void			editor_draw_line	(Editor* editor, int x, int y, SDL_Texture* texture, SDL_Color color);

#endif
