#include "editor.h"

Editor* editor_new(Window* window, char* file_name)
{
	Editor* editor = (Editor*) malloc(sizeof(Editor));
	editor->window = window;

	editor->file_name = file_name;
	editor->text_buffer = calloc(1, sizeof(char));
	editor->buffer_len= 0;

	// Buffer textures
	editor->editor_texture = NULL;
	editor->line_texture = NULL;
	editor->bar_texture = NULL;

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
	
	SDL_DestroyTexture(editor->editor_texture);
	SDL_DestroyTexture(editor->line_texture);
	SDL_DestroyTexture(editor->bar_texture);

	free(editor);
}

void editor_resize(Editor* editor)
{
	SDL_DestroyTexture(editor->editor_texture);
	SDL_DestroyTexture(editor->line_texture);
	SDL_DestroyTexture(editor->bar_texture);

	editor->editor_texture = sdl_check_ptr(SDL_CreateTexture(editor->window->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, editor->window->width , editor->window->height));
	editor->line_texture = sdl_check_ptr(SDL_CreateTexture(editor->window->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, editor->cur_w * 4, editor->window->height));
	editor->bar_texture = sdl_check_ptr(SDL_CreateTexture(editor->window->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, editor->window->width, editor->cur_h));
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
int	editor_get_cur_pos(Editor* editor, int x, int y)
{
	// Calculates the position of the cursor in the 1D text buffer
	int pos = 0;
	int index = 0;
	int line = 0;
	for (int i = 0; i < editor->buffer_len; i++)
	{
		if (line == y) break;
		if (editor->text_buffer[i] == '\n') 
		{
			pos += index + 1;
			index = 0;
			line++;
		}
		else
			index++;
	}
	pos += x;
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

int editor_line_len(Editor* editor, int line_no)
{
	int size = 0, start = 0, end = 0, lines = 0;
	for (int i = 0; i < editor->buffer_len; i++)
	{
		if ((editor->text_buffer[i] == '\n') || (i + 1 == editor->buffer_len))
		{	
			lines++;
			if (lines == line_no)
			{
				if (editor->buffer_len == end + 1) end++;
				size = end - start;
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
	return size;
}

void editor_get_line(Editor* editor, int line_no, char* out)
{
	// Gets the string of the particular line no
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
				memcpy(out, editor->text_buffer+start, size);
				strcpy(out + size, "\0");
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
}

void editor_insert(Editor* editor, char chr)
{
	char* new_buffer = calloc(editor->buffer_len + 1, sizeof(char));

	int pos = editor_get_cur_pos(editor, editor->cur_x, editor->cur_y);

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
	int pos = editor_get_cur_pos(editor, editor->cur_x, editor->cur_y) - 1;
	if (pos >= 0)
	{
		editor->cur_x--;
		editor->cur_rend_x--;

		// Jumping when there is tab
		if (editor->text_buffer[pos] == '\t')
		{
			editor->cur_rend_x -= TAB_SIZE - 1;
		}

		if (editor->cur_x < 0 || editor->cur_rend_x < 0)
		{
			editor_cur_up(editor);

			// Setting the cursor x pos to the last pos in the above line
			int size = editor_line_len(editor, editor->cur_y + 1);
			char line[size];
			editor_get_line(editor, editor->cur_y + 1, line);

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
		int pos = editor_get_cur_pos(editor, editor->cur_x, editor->cur_y);
		
		// Jumping when there is tab
		if (editor->text_buffer[pos] == '\t')
			editor->cur_rend_x -= TAB_SIZE - 1;
	}
}

void editor_cur_right(Editor* editor)
{
	int size = editor_line_len(editor, editor->cur_y + 1);
	char line[size];
	editor_get_line(editor, editor->cur_y + 1, line);

	int len = 0;

	if (line) 
	{
		len = strlen(line);
	}
	
	// Moving the cursor to right if only there is space
	if (editor->cur_x < len)
	{
		int pos = editor_get_cur_pos(editor, editor->cur_x, editor->cur_y);
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
		int size = editor_line_len(editor, editor->cur_y);
		char line[size];
		editor_get_line(editor, editor->cur_y, line);
		
		int len = 0;
		int len_2 = 0;

		if (line) 
		{
			len = strlen(line);
			for (int i = 0; i < len; i++)
			{
				if (line[i] == '\t')
					len_2 += TAB_SIZE;
				else
					len_2++;
			}
		}

		// Moving the cursor
		if (len < editor->cur_x)
		{
			editor->cur_x = len;
		}
		if (len_2 < editor->cur_rend_x)
		{
			editor->cur_rend_x = len_2;
		}

 		editor->cur_y--;
 		editor->cur_rend_y--;

		int pos = 0;
		for (int i = 0; i < len; i++)
		{
			if (line[i] != '\t')
				break;
			pos += TAB_SIZE;
		}

		if (0 <= editor->cur_rend_x && editor->cur_rend_x < pos)
		{
			editor->cur_rend_x += pos - editor->cur_rend_x;
			editor->cur_x = 0;
			editor->cur_x += pos / TAB_SIZE;
		}
		else if (editor->cur_rend_x != 0)
		{
			editor->cur_x = (pos / TAB_SIZE) + (editor->cur_rend_x - pos);
		}
	}
}

void editor_cur_down(Editor* editor)
{
	int line_no = editor_get_line_no(editor);
	if (editor->cur_y + 1 < line_no)
	{
		int size = editor_line_len(editor, editor->cur_y + 2);
		char line[size];
		editor_get_line(editor, editor->cur_y + 2, line);

		int len = 0;
		int len_2 = 0;
		
		if (line) 
		{
			len = strlen(line);
			for (int i = 0; i < len; i++)
			{
				if (line[i] == '\t')
					len_2 += TAB_SIZE;
				else
					len_2++;
			}
		}

		// Changing X axis
		if (len < editor->cur_x)
		{
			editor->cur_x = len;
		}
		if (len_2 < editor->cur_rend_x)
		{
			editor->cur_rend_x = len_2;
		}

		// Changing Y axis
		editor->cur_y++;
		editor->cur_rend_y++;

		// Calculating the tab size in the begining of the line
		int pos = 0;
		for (int i = 0; i < len; i++)
		{
			if (line[i] != '\t')
				break;
			pos += TAB_SIZE;
		}

		// Jumping if there is a tab
		if (0 <= editor->cur_rend_x && editor->cur_rend_x < pos)
		{
			editor->cur_rend_x += pos - editor->cur_rend_x;
			editor->cur_x = 0;
			editor->cur_x += pos / TAB_SIZE;
		}
		else if (editor->cur_rend_x != 0)
		{
			editor->cur_x = (pos / TAB_SIZE) + (editor->cur_rend_x - pos);
		}
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
void editor_render_buffer(Editor* editor, int start, int end, TTF_Font* font, SDL_Color fg, SDL_Color bg)
{
	// Setting up the render target for text buffer
	SDL_SetRenderTarget(editor->window->renderer, editor->editor_texture);
	SDL_SetRenderDrawColor(editor->window->renderer, bg.r, bg.g, bg.b, bg.a);
	SDL_RenderClear(editor->window->renderer);

	// Flushing the buffer into the render target
	for (int y = 0, i = start; i < end; y++, i++)
	{
		int size = editor_line_len(editor, i);
		char line[size];
		editor_get_line(editor, i, line);

		if (size > 0)
		{
			// Adding 4 bytes for the each tabs
			int buff_size = size;
			for (int i = 0; i < size; i++)
			{
				if (line[i] == '\t') buff_size += TAB_SIZE - 1;
			}

			char text[buff_size + 1];

			for (int k = 0, j = 0; j < size; j++)
			{
				char ch = line[j];
				if (ch == '\t')
				{
					for (int i = 0; i < TAB_SIZE; i++) text[k+i] = '-';
					k += TAB_SIZE;
				}
				else if (ch != '\n')
				{
					text[k] = line[j];
					k++;
				}
				else if (ch == '\n')
				{
					text[k] = ' ';
					k++;
				}
			}
			text[buff_size] = '\0';

			SDL_Texture* texture = create_texture(editor->window->renderer, font, text); 
			editor_draw_line(editor, -editor->scroll_x, y, texture, fg);
			SDL_DestroyTexture(texture);	
		}
	}

	// Rendering cursor
	SDL_Rect cur_rect = { (editor->cur_rend_x - editor->scroll_x) * editor->cur_w,  (editor->cur_rend_y - editor->scroll_y) * editor->cur_h , editor->cur_w, editor->cur_h };
	SDL_SetRenderDrawColor(editor->window->renderer, editor->cur_fg.r, editor->cur_fg.g, editor->cur_fg.b, editor->cur_fg.a);
	SDL_RenderFillRect(editor->window->renderer, &cur_rect); 

}

void editor_render_line(Editor* editor, int start, int end, TTF_Font* font, SDL_Color fg, SDL_Color bg)
{
	// Changing the target texture
	SDL_SetRenderTarget(editor->window->renderer, editor->line_texture);
	SDL_SetRenderDrawColor(editor->window->renderer, bg.r, bg.g, bg.b, bg.a);
	SDL_RenderClear(editor->window->renderer);

	for (int i = start; i < end; i++) 
	{
		char c[end-start + 1];
		sprintf(c, "%d\0", i);
		SDL_Texture* texture = sdl_check_ptr(create_texture(editor->window->renderer, font, c));
		draw_text(editor->window->renderer, 0, i - 1 - editor->scroll_y, texture, fg);
	    SDL_DestroyTexture(texture);
	}
}

void editor_render_bar(Editor* editor, TTF_Font* font, SDL_Color fg, SDL_Color bg)
{
	// Setting up the render target for the command buffer
	SDL_SetRenderTarget(editor->window->renderer, editor->bar_texture);
	SDL_SetRenderDrawColor(editor->window->renderer, bg.r, bg.g, bg.b, bg.a);
	SDL_RenderClear(editor->window->renderer);
	
	// Rendering the cursor position
	char cur_pos[5+1+5] = {0};
	sprintf(cur_pos, "%d:%d", editor->cur_x, editor->cur_y);

	SDL_Texture* cur_pos_texture = create_texture(editor->window->renderer, font, cur_pos);
    int w, h; 
	SDL_QueryTexture(cur_pos_texture, NULL, NULL, &w, &h);
	SDL_Rect pos = { editor->window->width - w - 5, 0, w, h };
	SDL_RenderCopy(editor->window->renderer, cur_pos_texture, NULL, &pos);
	SDL_DestroyTexture(cur_pos_texture);

	// Rendering file name
	SDL_Texture* file_name = create_texture(editor->window->renderer, font, editor->file_name);
	draw_text(editor->window->renderer, 0, 0, file_name, fg);
	SDL_DestroyTexture(file_name);
}

void editor_render(Editor* editor, Window* window, TTF_Font* font, SDL_Color fg, SDL_Color bg)
{
	// Calling scrolling functions
	editor_scroll_left (editor);
	editor_scroll_right(editor);
	editor_scroll_up   (editor);
	editor_scroll_down (editor);

	// Calculating the visible lines
	int total_line = editor_get_line_no(editor);
	int start = editor->scroll_y + 1;
	int end = editor->window->height / editor->cur_h + editor->scroll_y + 1;
	if (end > total_line) end = total_line;

	// Rendering buffers
	editor_render_line(editor, start, end, font, fg, bg);
	editor_render_buffer(editor, start, end, font, fg, bg);
	editor_render_bar(editor, font, fg, bg);

	// Defaulting the render target
	SDL_SetRenderTarget(window->renderer, NULL);

	// Rendering texture buffer
	SDL_Rect texture_rect = { editor->cur_w * 4, 0, window->width, window->height };
	SDL_RenderCopy(window->renderer, editor->editor_texture, NULL, &texture_rect);

	// Rendering line buffer
	SDL_Rect line_rect = { 0, 0, editor->cur_w * 4, window->height };
	SDL_RenderCopy(window->renderer, editor->line_texture, NULL, &line_rect);
	
	// Rendering command line buffer
	SDL_Rect cmd_line_rect = { 0, editor->window->height - editor->cur_h, editor->window->width, editor->cur_h };
	SDL_RenderCopy(window->renderer, editor->bar_texture, NULL, &cmd_line_rect);
}

void editor_gen_texture(Editor* editor, SDL_Renderer* renderer, TTF_Font* font)
{
	SDL_Texture* ch = create_texture(renderer, font, "A");
	SDL_QueryTexture(ch, NULL, NULL, &editor->cur_w, &editor->cur_h);
	SDL_DestroyTexture(ch);

	// Generating textures for the buffers
	editor->editor_texture = sdl_check_ptr(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, editor->window->width , editor->window->height));
	editor->line_texture = sdl_check_ptr(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, editor->cur_w * 4, editor->window->height));
	editor->bar_texture = sdl_check_ptr(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, editor->window->width, editor->cur_h));
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
