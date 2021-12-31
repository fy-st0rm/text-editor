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

	// Cursor
	int cur_x, cur_y, cur_w, cur_h;
	SDL_Color cur_fg;

	SDL_Texture** texture_cache;
	SDL_Texture* editor_texture;
} Editor;

Editor* 		editor_new			(Window* window);
void			editor_destroy		(Editor* editor);
void 			editor_resize		(Editor* editor, Window* window);

// Editor buffer management
int				editor_get_cur_pos	(Editor* editor);
void			editor_insert		(Editor* editor, char chr);

// Editor cursor
void			editor_cur_left		(Editor* editor);
void			editor_cur_right	(Editor* editor);

// Editor rendering
void			editor_render_text	(Editor* editor, Window* window, SDL_Color fg, SDL_Color bg);
void			editor_gen_tex_cache(Editor* editor, SDL_Renderer* renderer, TTF_Font* font);

// Render helper function
SDL_Texture* 	create_texture		(SDL_Renderer* renderer, TTF_Font* font, char* text);
void			draw_text			(SDL_Renderer* renderer, int x, int y, SDL_Texture* texture, SDL_Color color);

#endif
