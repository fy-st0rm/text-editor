#include "editor.h"

Editor* editor_new(Window* window)
{
	Editor* editor = (Editor*) malloc(sizeof(Editor));

	editor->text_buffer = calloc(1, sizeof(char));
	editor->buffer_len= 0;

	editor->texture_cache = malloc(sizeof(SDL_Texture*) * MAX_TEXTURE);
	editor->editor_texture = sdl_check_ptr(SDL_CreateTexture(window->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, window->width, window->height));

	editor->cur_x = 0;
	editor->cur_y = 0;
	editor->cur_w = 0;
	editor->cur_h = 0;
	SDL_Color cur_fg = { 255, 255, 255, 255 };
	editor->cur_fg = cur_fg;

	return editor;
}

void editor_destroy(Editor* editor)
{
	free(editor->text_buffer);
	for (int i = 0; i < MAX_TEXTURE; i++) SDL_DestroyTexture(editor->texture_cache[i]);
	free(editor->texture_cache);
	free(editor);
}

void editor_resize(Editor* editor, Window* window)
{
	SDL_DestroyTexture(editor->editor_texture);
	editor->editor_texture = sdl_check_ptr(SDL_CreateTexture(window->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, window->width, window->height));
}

// Editor buffer management
int	editor_get_cur_pos(Editor* editor)
{
	// Calculates the position of the cursor in the 1D text buffer
	int pos = 0;
	int index = 0;
	int line = 0;
	for (int i = 0; i < editor->buffer_len; i++)
	{
		if (editor->text_buffer[i] == '\n') 
		{
			pos += index + 1;
			index = 0;
			line++;
		}
		else
			index++;
		if (line == editor->cur_y) break;
	}
	pos += editor->cur_x;
	return pos;
}

void editor_insert(Editor* editor, char chr)
{
	printf("Char %c\n", chr);
	char* new_buffer = calloc(editor->buffer_len + 1, sizeof(char));

	printf("Calc pos\n");
	int pos = editor_get_cur_pos(editor);
	printf("Pos: %d Buff_len: %d\n", pos, editor->buffer_len);

	// Inserting character when the cursor is at last position
	if (pos == editor->buffer_len)
	{
		strcpy(new_buffer, editor->text_buffer);
		new_buffer[pos] = chr;
	}
	// Inserting character when cursor is in between of buffer
	else
	{
		memcpy(new_buffer, editor->text_buffer, pos);
		char c[1] = {0};
		sprintf(c, "%c", chr);
		strcpy(new_buffer + pos, c);
		memcpy(new_buffer + pos + 1, editor->text_buffer + pos, editor->buffer_len - pos);
	}

	free(editor->text_buffer);
	
	editor->text_buffer = new_buffer;
	editor->buffer_len++;
	editor->cur_x++;

	if (chr == '\n')
	{
		editor->cur_x = 0;
		editor->cur_y++;
	}
}

void editor_backspace(Editor* editor)
{
	int pos = editor_get_cur_pos(editor);
	if (pos)
	{
		// overiting the memory in that position with the memory infront of that position
		memmove(&editor->text_buffer[pos], &editor->text_buffer[pos+1], editor->buffer_len - pos);
		editor->buffer_len--;
		editor_cur_left(editor);
	}
}

// Editor cursor
void editor_cur_left(Editor* editor)
{
	if (editor->cur_x > 0) editor->cur_x--;
}

void editor_cur_right(Editor* editor)
{
	char* temp_buffer = calloc(editor->buffer_len, sizeof(char));
	strcpy(temp_buffer, editor->text_buffer);

	char* line = strtok(temp_buffer, "\n");
	for (int i = 0; i < editor->cur_y; i++)
		line = strtok(NULL, "\n");
	
	if ((line) && (editor->cur_x < strlen(line)))
		editor->cur_x++;

	free(temp_buffer);
}

// Editor rendering
void editor_render_text(Editor* editor, Window* window, SDL_Color fg, SDL_Color bg)
{
	// Setting up the render target
	SDL_SetRenderTarget(window->renderer, editor->editor_texture);
	SDL_SetRenderDrawColor(window->renderer, bg.r, bg.g, bg.b, bg.a);
	SDL_RenderClear(window->renderer);

	// Flushing the buffer into the render target
	int x = 0, y = 0;
	for (int i = 0; i < editor->buffer_len; i++)
	{
		int index = editor->text_buffer[i];
		if (index == 10) // 10 == \n When changing the line
		{
			x = 0;
			y++;
		}
		else
		{
			draw_text(window->renderer, x, y, editor->texture_cache[index - CHAR_START], fg); 
			x++;
		}
	}

	// Rendering cursor
	SDL_Rect cur_rect = { editor->cur_x * editor->cur_w, editor->cur_y * editor->cur_h, editor->cur_w, editor->cur_h };
	SDL_SetRenderDrawColor(window->renderer, editor->cur_fg.r, editor->cur_fg.g, editor->cur_fg.b, editor->cur_fg.a);
	SDL_RenderFillRect(window->renderer, &cur_rect); 
	
	// Switching the render target to the screen and rendering the texture into the screen
	SDL_SetRenderTarget(window->renderer, NULL);
	SDL_Rect texture_rect = { 0, 0, window->width, window->height };
	SDL_RenderCopy(window->renderer, editor->editor_texture, NULL, &texture_rect);
}

void editor_gen_tex_cache(Editor* editor, SDL_Renderer* renderer, TTF_Font* font)
{
	// Loading from 32 = ' ' to 126 = '`'
	for (int i = CHAR_START; i < CHAR_END; i++)
	{
		char ch = i;
		char text[1];
		sprintf(text, "%c", ch);
		editor->texture_cache[i-CHAR_START] = create_texture(renderer, font, text);
	}

	SDL_QueryTexture(editor->texture_cache[0], NULL, NULL, &editor->cur_w, &editor->cur_h);
}

SDL_Texture* create_texture(SDL_Renderer* renderer, TTF_Font* font, char* text)
{
	SDL_Color white = {255, 255, 255, 255};
	SDL_Surface* surface = sdl_check_ptr(TTF_RenderUTF8_Blended(font, text, white));
	SDL_Texture* texture = sdl_check_ptr(SDL_CreateTextureFromSurface(renderer, surface));

	SDL_FreeSurface(surface);
	return texture;
}	

void draw_text (SDL_Renderer* renderer, int x, int y, SDL_Texture* texture, SDL_Color color)
{
	SDL_Rect rect;
	SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);

	rect.x = x * rect.w;
	rect.y = y * rect.h;

	SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
	SDL_RenderCopy(renderer, texture, NULL, &rect);
}
