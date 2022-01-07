#include "editor.h"

Editor* editor_new(Window* window, char* file_name)
{
	Editor* editor = (Editor*) malloc(sizeof(Editor));
	editor->window = window;

	editor->file_name = file_name;
	editor->text_buffer = calloc(1, sizeof(char));
	editor->buffer_len= 0;

	editor->texture_cache = malloc(sizeof(SDL_Texture*) * MAX_TEXTURE);
	editor->editor_texture = NULL;
	editor->line_texture = NULL;
	editor->command_texture = NULL;

	// Cursor
	editor->cur_x = 0;
	editor->cur_y = 0;
	editor->cur_w = 0;
	editor->cur_h = 0;
	editor->cur_rend_x = 0;
	editor->cur_rend_y = 0;
	SDL_Color cur_fg = { 255, 255, 255, 255 };
	editor->cur_fg = cur_fg;

	// Scroll
	editor->scroll_x = 0;
	editor->scroll_y = 0;

	return editor;
}

void editor_destroy(Editor* editor)
{
	free(editor->text_buffer);
	for (int i = 0; i < MAX_TEXTURE; i++) SDL_DestroyTexture(editor->texture_cache[i]);
	free(editor->texture_cache);
	
	SDL_DestroyTexture(editor->editor_texture);
	SDL_DestroyTexture(editor->line_texture);
	SDL_DestroyTexture(editor->command_texture);

	free(editor);
}

void editor_resize(Editor* editor, Window* window)
{
	SDL_DestroyTexture(editor->editor_texture);
	SDL_DestroyTexture(editor->line_texture);
	SDL_DestroyTexture(editor->command_texture);

	editor->editor_texture = sdl_check_ptr(SDL_CreateTexture(window->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, window->width , window->height));
	editor->line_texture = sdl_check_ptr(SDL_CreateTexture(window->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, editor->cur_w * 4, window->height));
	editor->command_texture = sdl_check_ptr(SDL_CreateTexture(window->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, window->width, editor->cur_h));
}

// Editor file handling
void editor_read_file(Editor* editor)
{
	FILE* file;
	file = fopen(editor->file_name, "r");

	if (file == NULL)
		return;

	char ch;
	while((ch = fgetc(file)) != EOF)
		editor_insert(editor, ch);

	editor->cur_x = editor->cur_y = editor->cur_rend_x = editor->cur_rend_y = 0;
	fclose(file);
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
		if (line == editor->cur_y) break;
		if (editor->text_buffer[i] == '\n') 
		{
			pos += index + 1;
			index = 0;
			line++;
		}
		else
			index++;
	}
	pos += editor->cur_x;
	return pos;
}

int editor_get_line_no(Editor* editor)
{
	int line_no = 1;
	for (int i = 0; i < editor->buffer_len; i++)
	{
		if (editor->text_buffer[i] == '\n') line_no++;
	}
	return line_no;
}

char* editor_get_line(Editor* editor, int line_no)
{
	// Gets the string of the particular line no
	char* line = calloc(1, sizeof(char));
	int start = 0, end = 0, lines = 0;
	for (int i = 0; i < editor->buffer_len; i++)
	{
		if ((editor->text_buffer[i] == '\n') || (i + 1 == editor->buffer_len))
		{	
			lines++;
			if (lines == line_no)
			{
				if (editor->buffer_len == end + 1) end++;
				int size = end - start;
				line = calloc(size + 1, sizeof(char));
				memcpy(line, editor->text_buffer+start, size);
				strcpy(line + size, "\0");
				break;
			}
			else
			{
				end++;
				start = end;
			}
		}
		else
		{
			end++;
		}
	}
	return line;
}

void editor_insert(Editor* editor, char chr)
{
	char* new_buffer = calloc(editor->buffer_len + 1, sizeof(char));

	int pos = editor_get_cur_pos(editor);

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
	editor->cur_rend_x++;

	// When user presses enter
	if (chr == '\n')
	{
		editor->cur_x = 0;
		editor->cur_rend_x = 0;
		editor->cur_y++;
		editor->cur_rend_y++;
	}
	// When user presses tab
	else if (chr == '\t')
	{
		editor->cur_rend_x += TAB_SIZE - 1;
	}
}

void editor_backspace(Editor* editor)
{
	int pos = editor_get_cur_pos(editor) - 1;
	if (pos >= 0)
	{
		editor->cur_x--;
		editor->cur_rend_x--;

		// Jumping when there is tab
		if (editor->text_buffer[pos] == '\t')
		{
			editor->cur_rend_x -= TAB_SIZE - 1;
		}

		if (editor->cur_x < 0)
		{
			editor_cur_up(editor);
			// Setting the cursor x pos to the last pos in the above line
			char* line = editor_get_line(editor, editor->cur_y + 1);
			int len = 0;
			if (line) 
			{
				len = strlen(line);
			}
			editor->cur_x = len;
			editor->cur_rend_x = len;

			// Counting tabs
			for (int i = 0; i < len; i++)
			{
				if (line[i] == '\t') editor->cur_rend_x += TAB_SIZE - 1;
			}
			
			free(line);
		}
		// overiting the memory in that position with the memory infront of that position
		memmove(&editor->text_buffer[pos], &editor->text_buffer[pos+1], editor->buffer_len - pos);
		editor->buffer_len--;
	}
}

// Editor cursor
void editor_cur_left(Editor* editor)
{
	if (editor->cur_x > 0) 
	{
		editor->cur_x--;
		editor->cur_rend_x--;
		int pos = editor_get_cur_pos(editor);
		
		// Jumping when there is tab
		if (editor->text_buffer[pos] == '\t')
			editor->cur_rend_x -= TAB_SIZE - 1;
	}
}

void editor_cur_right(Editor* editor)
{
	char* line = editor_get_line(editor, editor->cur_y + 1);
	int len = 0;

	if (line) 
	{
		len = strlen(line);
		free(line);
	}
	
	// Moving the cursor to right if only there is space
	if (editor->cur_x < len)
	{
		int pos = editor_get_cur_pos(editor);
		editor->cur_x++;
		editor->cur_rend_x++;

		// Jumping when there is tab
		if (editor->text_buffer[pos] == '\t')
			editor->cur_rend_x += TAB_SIZE - 1;
	}
}

void editor_cur_up(Editor* editor)
{
	if (editor->cur_y > 0)
	{
		char* line = editor_get_line(editor, editor->cur_y);
		int len = 0;

		if (line) 
		{
			len = strlen(line);
			free(line);
		}

		// Moving the cursor
		if (len < editor->cur_x)
		{
			editor->cur_x = len;
			editor->cur_rend_x = len;
		}
 		editor->cur_y--;
 		editor->cur_rend_y--;
	}
}

void editor_cur_down(Editor* editor)
{
	int line_no = editor_get_line_no(editor);
	if (editor->cur_y + 1 < line_no)
	{
		char* line = editor_get_line(editor, editor->cur_y + 2);
		int len = 0;
		
		if (line) 
		{
			len = strlen(line);
			free(line);
		}

		if (len < editor->cur_x)
		{
			editor->cur_x = len;
			editor->cur_rend_x = len;
		}
		editor->cur_y++;
		editor->cur_rend_y++;
	}
}

// Editor scrolls
void editor_scroll_left(Editor* editor)
{
	if (editor->cur_rend_x - editor->scroll_x < 0)
	{
		editor->scroll_x += editor->cur_rend_x - editor->scroll_x;
	}
}

void editor_scroll_right(Editor* editor)
{
	int w;
	SDL_QueryTexture(editor->editor_texture, NULL, NULL, &w, NULL);
	if (editor->cur_rend_x - editor->scroll_x >= (w - editor->cur_w * 4) / editor->cur_w)
	{
		int diff = ((w - editor->cur_w * 4)/ editor->cur_w) - (editor->cur_rend_x - editor->scroll_x) - 1;
		editor->scroll_x -= diff;
	}
}

void editor_scroll_up(Editor* editor)
{
	if ((editor->cur_y - editor->scroll_y < 0) && (editor->scroll_y > 0))
		editor->scroll_y += editor->cur_y - editor->scroll_y;
}

void editor_scroll_down(Editor* editor)
{
	int h;
	SDL_QueryTexture(editor->editor_texture, NULL, NULL, NULL, &h);
	if (editor->cur_y - editor->scroll_y > (h - editor->cur_h)  / editor->cur_h)
	{
		int diff = ((h - editor->cur_h) / editor->cur_h) - (editor->cur_y - editor->scroll_y);
		editor->scroll_y -= diff;
	}
}

// Editor rendering
void editor_render_text(Editor* editor, Window* window, TTF_Font* font, SDL_Color fg, SDL_Color bg)
{
	// Calling scrolling functions
	editor_scroll_left (editor);
	editor_scroll_right(editor);
	editor_scroll_up   (editor);
	editor_scroll_down (editor);

	// Setting up the render target for line buffer
	SDL_SetRenderTarget(window->renderer, editor->line_texture);
	SDL_SetRenderDrawColor(window->renderer, bg.r, bg.g, bg.b, bg.a);
	SDL_RenderClear(window->renderer);

	// Rendering only visible line numbers
	int total_line = editor_get_line_no(editor);
	int start = editor->scroll_y + 1;
	int end = editor->window->height / editor->cur_h + editor->scroll_y;
	if (end > total_line) end = total_line;

	for (int i = start; i < end + 1; i++) 
	{
		char c[end-start+1];
		sprintf(c, "%d", i);
		SDL_Texture* texture = sdl_check_ptr(create_texture(window->renderer, font, c));
		draw_text(window->renderer, 0, i - 1 - editor->scroll_y, texture, fg);
	    SDL_DestroyTexture(texture);	
	}

	// Setting up the render target for text buffer
	SDL_SetRenderTarget(window->renderer, editor->editor_texture);
	SDL_SetRenderDrawColor(window->renderer, bg.r, bg.g, bg.b, bg.a);
	SDL_RenderClear(window->renderer);

	// Flushing the buffer into the render target
	for (int y = 0, i = start; i < end + 1; y++, i++)
	{
		char* line = editor_get_line(editor, i);
		int len = strlen(line);

		if (len > 0)
		{
			// Adding 4 bytes for the each tabs
			int buff_size = len;
			for (int i = 0; i < len; i++)
			{
				if (line[i] == '\t') buff_size += TAB_SIZE;
			}
			char* text = calloc(buff_size, sizeof(char));

			for (int k = 0, j = 0; j < len; j++)
			{
				char ch = line[j];
				if (ch == '\t')
				{
					for (int i = 0; i < TAB_SIZE; i++) text[k+i] = ' ';
					k += TAB_SIZE;
				}
				else if (ch != '\n')
				{
					text[k] = line[j];
					k++;
				}
			}

			SDL_Texture* texture = create_texture(window->renderer, font, text); 
			editor_draw_line(editor, -editor->scroll_x, y, texture, fg);
			SDL_DestroyTexture(texture);	
			free(text);
		}
		free(line);
	}

	/*
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
			draw_text(window->renderer, x - editor->scroll_x, y - editor->scroll_y, editor->texture_cache[index - CHAR_START], fg);
			x++;
		}
	}*/

	// Rendering cursor
	SDL_Rect cur_rect = { (editor->cur_rend_x - editor->scroll_x) * editor->cur_w,  (editor->cur_rend_y - editor->scroll_y) * editor->cur_h , editor->cur_w, editor->cur_h };
	SDL_SetRenderDrawColor(window->renderer, editor->cur_fg.r, editor->cur_fg.g, editor->cur_fg.b, editor->cur_fg.a);
	SDL_RenderFillRect(window->renderer, &cur_rect); 

	// Setting up the render target for the command buffer
	SDL_SetRenderTarget(window->renderer, editor->command_texture);
	SDL_SetRenderDrawColor(window->renderer, bg.r, bg.g, bg.b, bg.a);
	SDL_RenderClear(window->renderer);
	
	// Rendering the cursor position
	char cur_pos[5+1+5] = {0};
	sprintf(cur_pos, "%d:%d", editor->cur_x, editor->cur_y);

	SDL_Texture* cur_pos_texture = create_texture(window->renderer, font, cur_pos);
    int w, h; 
	SDL_QueryTexture(cur_pos_texture, NULL, NULL, &w, &h);
	SDL_Rect pos = { window->width - w - 5, 0, w, h };
	SDL_RenderCopy(window->renderer, cur_pos_texture, NULL, &pos);
	SDL_DestroyTexture(cur_pos_texture);

	// Rendering file name
	SDL_Texture* file_name = create_texture(window->renderer, font, editor->file_name);
	draw_text(window->renderer, 0, 0, file_name, fg);
	SDL_DestroyTexture(file_name);

	// Switching the render target to the screen and rendering the texture into the screen
	SDL_SetRenderTarget(window->renderer, NULL);

	// Rendering texture buffer
	SDL_Rect texture_rect = { editor->cur_w * 4, 0, window->width, window->height };
	SDL_RenderCopy(window->renderer, editor->editor_texture, NULL, &texture_rect);

	// Rendering line buffer
	SDL_Rect line_rect = { 0, 0, editor->cur_w * 4, window->height };
	SDL_RenderCopy(window->renderer, editor->line_texture, NULL, &line_rect);
	
	// Rendering command line buffer
	SDL_Rect cmd_line_rect = { 0, editor->window->height - editor->cur_h, editor->window->width, editor->cur_h };
	SDL_RenderCopy(window->renderer, editor->command_texture, NULL, &cmd_line_rect);
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

	// Generating textures for the buffers
	editor->editor_texture = sdl_check_ptr(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, editor->window->width , editor->window->height));
	editor->line_texture = sdl_check_ptr(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, editor->cur_w * 4, editor->window->height));
	editor->command_texture = sdl_check_ptr(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, editor->window->width, editor->cur_h));
}

void editor_draw_line(Editor* editor, int x, int y, SDL_Texture* texture, SDL_Color color)
{
	SDL_Rect rect;
	SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);

	rect.x = x * editor->cur_w;
	rect.y = y * rect.h;

	SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
	SDL_RenderCopy(editor->window->renderer, texture, NULL, &rect);
}

SDL_Texture* create_texture(SDL_Renderer* renderer, TTF_Font* font, char* text)
{
	SDL_Color white = {255, 255, 255, 255};
	SDL_Surface* surface = sdl_check_ptr(TTF_RenderUTF8_Blended(font, text, white));
	SDL_Texture* texture = sdl_check_ptr(SDL_CreateTextureFromSurface(renderer, surface));

	SDL_FreeSurface(surface);
	return texture;
}	

void draw_text(SDL_Renderer* renderer, int x, int y, SDL_Texture* texture, SDL_Color color)
{
	SDL_Rect rect;
	SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);

	rect.x = x * rect.w;
	rect.y = y * rect.h;

	SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
	SDL_RenderCopy(renderer, texture, NULL, &rect);
}
