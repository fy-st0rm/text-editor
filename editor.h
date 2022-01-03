#ifndef _EDITOR_H_
#define _EDITOR_H_

#include "util.h"
#include "window.h"

#define MAX_TEXTURE 93
#define CHAR_START  32
#define CHAR_END	127

typedef struct
{
	char chr;
	SDL_Color fg, bg;
} Cell;

typedef struct
{
	char* text_buffer;
	int buffer_len;
	Window* window;

	// Cursor
	int cur_x, cur_y, cur_w, cur_h;
	SDL_Color cur_fg;
	
	// Scroll
	int scroll_x, scroll_y;

	SDL_Texture** texture_cache;
	SDL_Texture* editor_texture;
	SDL_Texture* line_texture;
	SDL_Texture* command_texture;
} Editor;

Editor* 		editor_new			(Window* window);
void			editor_destroy		(Editor* editor);
void 			editor_resize		(Editor* editor, Window* window);

// Editor buffer management
int				editor_get_cur_pos	(Editor* editor);
int				editor_get_line_no	(Editor* editor);
char*			editor_get_line		(Editor* editor, int line_no);
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
void			editor_render_text	(Editor* editor, Window* window, TTF_Font* font, SDL_Color fg, SDL_Color bg);
void			editor_gen_tex_cache(Editor* editor, SDL_Renderer* renderer, TTF_Font* font);

// Render helper function
SDL_Texture* 	create_texture		(SDL_Renderer* renderer, TTF_Font* font, char* text);
void			draw_text			(SDL_Renderer* renderer, int x, int y, SDL_Texture* texture, SDL_Color color);

#endif
