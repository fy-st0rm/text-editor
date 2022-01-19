#include "editor.h"


Editor* editor_new(Window* window, Settings* settings, char* file_name, bool modifiable)
{
	Editor* editor = (Editor*) malloc(sizeof(Editor));
	editor->window = window;
	editor->settings = settings;
	editor->font_1 = sdl_check_ptr(TTF_OpenFont(settings->family1, settings->font_size_1));
	editor->font_2 = sdl_check_ptr(TTF_OpenFont(settings->family2, settings->font_size_2));

	strcpy(editor->file_name, file_name);
	editor->text_buffer = calloc(2, sizeof(char));
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

	// Selection
	editor->sel_start = 0;
	editor->sel_end = 0;
	editor->sel_init = 0;

	// Scroll
	editor->scroll_x = 0;
	editor->scroll_y = 0;

	// Flags
	editor->modifiable = modifiable;
	editor->edited = false;
	editor->norm_visual = false;
	editor->line_visual = false;

	// Syntax stuff
	editor->syntax_on = editor->settings->syntax_on;
	editor->data_types_len = 0;
	editor->data_types = NULL;
	editor->keywords_len = 0;
	editor->keywords = NULL;
	editor->op_statements_len = 0;
	editor->op_statements = NULL;
	editor->functions_len = 0;
	editor->functions = NULL;
	editor->bools = NULL;

	editor_gen_texture(editor, window->renderer, editor->font_1);
	editor_read_file(editor, editor->file_name);

	return editor;
}

void editor_destroy(Editor* editor)
{
	free(editor->text_buffer);
	
	SDL_DestroyTexture(editor->editor_texture);
	SDL_DestroyTexture(editor->line_texture);
	SDL_DestroyTexture(editor->bar_texture);

	free(editor);
	editor->text_buffer = NULL;
	editor->editor_texture = NULL;
	editor->line_texture = NULL;
	editor->bar_texture = NULL;
	editor = NULL;
}

void editor_resize(Editor* editor)
{
	SDL_DestroyTexture(editor->editor_texture);
	SDL_DestroyTexture(editor->line_texture);
	SDL_DestroyTexture(editor->bar_texture);

	editor->editor_texture = sdl_check_ptr(SDL_CreateTexture(editor->window->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, editor->window->width , editor->window->height));
	editor->line_texture = sdl_check_ptr(SDL_CreateTexture(editor->window->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, editor->cur_w * 6, editor->window->height));

	SDL_Texture* ch = create_texture(editor->window->renderer, editor->font_2, "A");
	int w, h;
	SDL_QueryTexture(ch, NULL, NULL, &w, &h);
	SDL_DestroyTexture(ch);

	editor->bar_texture = sdl_check_ptr(SDL_CreateTexture(editor->window->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, editor->window->width, h));
}

// Editor file handling
int editor_read_file(Editor* editor, char* file_name)
{
	if (strlen(file_name) > 0)
	{
		FILE* file;
		file = fopen(file_name, "r");

		if (file == NULL)
			return 2;

		editor->cur_x = editor->cur_y = editor->cur_rend_x = editor->cur_rend_y = 0;
		editor->buffer_len = 0;
		strcpy(editor->text_buffer, "\0");

		// Reading file line by line
		char* line = NULL;
		size_t len = 0;
		ssize_t read;

		while ((read = getline(&line, &len, file)) != -1)
		{
			editor_insert_str(editor, line);
		}

		fclose(file);
		editor->cur_x = editor->cur_y = editor->cur_rend_x = editor->cur_rend_y = 0;
		strcpy(editor->file_name, file_name);

		// Checking the file format
		if ((strstr(file_name, ".c") != NULL) || (strstr(file_name, ".cpp") != NULL) || (strstr(file_name, ".h") != NULL))
		{
			editor->data_types_len = c_data_types_len;
			editor->data_types = c_data_types;
			editor->keywords_len = c_keywords_len;
			editor->keywords = c_keywords;
			editor->op_statements_len = c_op_statements_len;
			editor->op_statements = c_op_statements;
			editor->functions_len = c_functions_len;
			editor->functions = c_functions;
			editor->bools = c_bools;
		}
		else if (strstr(file_name, ".py"))
		{
			editor->data_types_len = py_data_types_len;
			editor->data_types = py_data_types;
			editor->keywords_len = py_keywords_len;
			editor->keywords = py_keywords;
			editor->op_statements_len = py_op_statements_len;
			editor->op_statements = py_op_statements;
			editor->functions_len = py_functions_len;
			editor->functions = py_functions;
			editor->bools = py_bools;
		}
		else
		{
			editor->data_types_len = 0;
			editor->data_types = NULL;
			editor->keywords_len = 0;
			editor->keywords = NULL;
			editor->op_statements_len = 0;
			editor->op_statements = NULL;
			editor->functions_len = 0;
			editor->functions = NULL;
			editor->bools = NULL;
		}

		editor->edited = false;
		return 3;
	}
	return 1;
}

int editor_write_file(Editor* editor, char* file_name)
{
	if (strlen(file_name) > 0)
	{
		FILE* file;
		file = fopen(file_name, "w");

		if (file == NULL)
			return 2;

		fprintf(file, "%s", editor->text_buffer);
		fclose(file);

		strcpy(editor->file_name, file_name);

		// Checking the file format
		if ((strstr(file_name, ".c") != NULL) || (strstr(file_name, ".cpp") != NULL) || (strstr(file_name, ".h") != NULL))
		{
			editor->data_types_len = c_data_types_len;
			editor->data_types = c_data_types;
			editor->keywords_len = c_keywords_len;
			editor->keywords = c_keywords;
			editor->op_statements_len = c_op_statements_len;
			editor->op_statements = c_op_statements;
			editor->functions_len = c_functions_len;
			editor->functions = c_functions;
			editor->bools = c_bools;
		}
		else if (strstr(file_name, ".py"))
		{
			editor->data_types_len = py_data_types_len;
			editor->data_types = py_data_types;
			editor->keywords_len = py_keywords_len;
			editor->keywords = py_keywords;
			editor->op_statements_len = py_op_statements_len;
			editor->op_statements = py_op_statements;
			editor->functions_len = py_functions_len;
			editor->functions = py_functions;
			editor->bools = py_bools;
		}
		else
		{
			editor->data_types_len = 0;
			editor->data_types = NULL;
			editor->keywords_len = 0;
			editor->keywords = NULL;
			editor->op_statements_len = 0;
			editor->op_statements = NULL;
			editor->functions_len = 0;
			editor->functions = NULL;
			editor->bools = NULL;
		}

		editor->edited = false;
		return 0;
	}
	return 1; 
}

// Font handling
void editor_change_font(Editor* editor, int type, char* family, int size)
{
	if (type == 0)
	{
		TTF_CloseFont(editor->font_1);
		editor->font_1 = sdl_check_ptr(TTF_OpenFont(family, size));
		editor_update_cursor(editor);
	}
	else if (type == 1)
	{
		TTF_CloseFont(editor->font_2);
		editor->font_2 = sdl_check_ptr(TTF_OpenFont(family, size));
	}
	editor_resize(editor);
}

void editor_update_cursor(Editor* editor)
{
	SDL_Texture* ch = create_texture(editor->window->renderer, editor->font_1, "A");
	SDL_QueryTexture(ch, NULL, NULL, &editor->cur_w, &editor->cur_h);
	SDL_DestroyTexture(ch);
}

// Editor buffer management
int	editor_get_cur_pos(Editor* editor, int x, int y)
{
	int pos = 0, end = 0;

	char* line = strchr(editor->text_buffer, '\n');
	if (line != NULL)
		end = line - editor->text_buffer;
	else
		end = editor->buffer_len;
	line++;

	for (int i = 0; i < y; i++)
	{
		pos = end + 1;
		line = strchr(line, '\n');
		if (line != NULL)
			end = line - editor->text_buffer;
		else
			end = editor->buffer_len;
		line++;
	}
	pos += x;
	return pos;
}

void editor_set_cur_pos(Editor* editor, int pos)
{
	if (pos <= editor->buffer_len)
	{
		editor->cur_x = editor->cur_y = 0;
		editor->cur_rend_x = editor->cur_rend_y = 0;

		for (int i = 0; i < pos; i++)
		{
			char ch = editor->text_buffer[i];
			if (ch == '\n')
			{
				editor->cur_y++;
				editor->cur_rend_y++;
				editor->cur_x = editor->cur_rend_x = 0;
			}
			else if (ch == '\t')
			{
				editor->cur_x++;
				editor->cur_rend_x += TAB_SIZE;
			}
			else
			{
				editor->cur_x++;
				editor->cur_rend_x++;
			}
		}
	}
}

void editor_set_cur_back(Editor* editor)
{
	int pos = editor_get_cur_pos(editor, editor->cur_x, editor->cur_y);
	for (int i = pos; i <= editor->buffer_len; i++)
	{
		char ch = editor->text_buffer[i];
		if (ch == '\n')
		{
			editor_set_cur_pos(editor, i);
			break;
		}
		else if (i == editor->buffer_len)
		{
			editor_set_cur_pos(editor, i);
			break;
		}
	}
}

void editor_set_cur_front(Editor* editor)
{
	int pos = editor_get_cur_pos(editor, editor->cur_x, editor->cur_y);
	for (int i = pos - 1; i >= 0; i--) 
	{
		char ch = editor->text_buffer[i];
		if (ch == '\n')
		{
			editor_set_cur_pos(editor, i + 1);
			break;
		}
		else if (i == 0)
		{
			editor_set_cur_pos(editor, i);
		}
	}
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
	int start = 0, end = 0;

	char* line = strchr(editor->text_buffer, '\n');
	if (line != NULL)
		end = line - editor->text_buffer;
	else
		end = editor->buffer_len;
	line++;

	for (int i = 1; i < line_no; i++)
	{
		line = strchr(line, '\n');
		start = ++end;
		if (line != NULL)
			end = line - editor->text_buffer;
		else
			end = editor->buffer_len;
		line++;
	}

	int size = end - start;
	return size;
}

void editor_get_line(Editor* editor, int line_no, char* out)
{
	int start = 0, end = 0;

	char* line = strchr(editor->text_buffer, '\n');
	if (line != NULL)
		end = line - editor->text_buffer;
	else
		end = editor->buffer_len;
	line++;

	for (int i = 1; i < line_no; i++)
	{
		line = strchr(line, '\n');
		start = ++end;
		if (line != NULL)
			end = line - editor->text_buffer;
		else
			end = editor->buffer_len;
		line++;
	}

	int size = end - start;
	memcpy(out, editor->text_buffer + start, size);
	strcpy(out + size, "\0");
}

// Insert functions
void editor_insert(Editor* editor, char chr)
{
	char* new_buffer = calloc(editor->buffer_len + 2, sizeof(char));
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
	strcpy(editor->text_buffer + editor->buffer_len, "\0");

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

	editor->edited = true;
}

void editor_insert_str(Editor* editor, char* text)
{
	char str[strlen(text)];
	strcpy(str, text);

	for (int i = 0; i < strlen(str); i++)
	{
		editor_insert(editor, text[i]);
	}
}

void editor_insert_nl_bel(Editor* editor)
{
	int pos = editor_get_cur_pos(editor, editor->cur_x, editor->cur_y);
	for (int i = pos; i <= editor->buffer_len; i++)
	{
		char ch = editor->text_buffer[i];
		if (ch == '\n')
		{
			editor_set_cur_pos(editor, i);
			editor_insert(editor, '\n');
			break;
		}
		else if (i == editor->buffer_len)
		{
			editor_set_cur_pos(editor, i);
			editor_insert(editor, '\n');
			break;
		}
	}
}

void editor_insert_nl_abv(Editor* editor)
{
	int pos = editor_get_cur_pos(editor, editor->cur_x, editor->cur_y) - 1;
	for (int i = pos; i >= 0; i--) 
	{
		char ch = editor->text_buffer[i];
		if (ch == '\n')
		{
			editor_set_cur_pos(editor, i);
			editor_insert(editor, '\n');
			break;
		}
		else if (i == 0)
		{
			editor_set_cur_pos(editor, i);
			editor_insert(editor, '\n');
			editor_set_cur_pos(editor, i);
			break;
		}
	}
}

void editor_replace_sel(Editor* editor)
{
	editor_delete_sel(editor);
	editor_paste(editor);
}

void editor_auto_indent(Editor* editor)
{
	// Auto indent
	int size = editor_line_len(editor, editor->cur_y);
	char line[size];
	editor_get_line(editor, editor->cur_y, line);

	for(int i = 0; i < size; i++)
	{
		char ch = line[i];
		if (ch == ' ' || ch == '\t')
			editor_insert(editor, ch);
		else
			break;
	}
}

// Delete functions
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

			int total_line = editor_get_line_no(editor);
			if (editor->cur_y + 1 == total_line - 1)
			{
				editor->cur_rend_x = len - 1;
				editor->cur_x = len - 1;
			}
			else
			{
				editor->cur_rend_x = len;
				editor->cur_x = len;
			}

			// Counting tabs
			for (int i = 0; i < len; i++)
			{
				if (line[i] == '\t') editor->cur_rend_x += TAB_SIZE - 1;
			}
		}

		// overiting the memory in that position with the memory infront of that position
		memmove(&editor->text_buffer[pos], &editor->text_buffer[pos+1], editor->buffer_len - pos);
		if (editor->buffer_len > 0)
			editor->buffer_len--;
		editor->edited = true;
	}
}

void editor_delete(Editor* editor)
{
	int pos = editor_get_cur_pos(editor, editor->cur_x, editor->cur_y);
	if (pos >= 0 && pos < editor->buffer_len)
	{
		// overiting the memory in that position with the memory infront of that position
		memmove(&editor->text_buffer[pos], &editor->text_buffer[pos+1], editor->buffer_len - pos);
		if (editor->buffer_len > 0)
			editor->buffer_len--;
	}
}

void editor_delete_line(Editor* editor)
{
	int start = 0, end = 0, lines = 0;
	for (int i = 0; i < editor->buffer_len; i++)
	{
		if ((editor->text_buffer[i] == '\n') || (i + 1 == editor->buffer_len))
		{	
			lines++;
			if (lines == editor->cur_y + 1)
			{	
				end++;
				if (editor->buffer_len == end + 1) end++;
				int size = end - start;

				// Deleting the line
				char* new_buffer = calloc(editor->buffer_len, sizeof(char));
				memcpy(new_buffer, editor->text_buffer, start);
				strcpy(new_buffer + start, editor->text_buffer + end);
				free(editor->text_buffer);
				editor->text_buffer = new_buffer;

				editor->edited = true;
				editor->cur_x = editor->cur_rend_x = 0;
				editor->buffer_len -= size;

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

void editor_delete_left(Editor* editor)
{
	int pos = editor_get_cur_pos(editor, editor->cur_x, editor->cur_y) - 1;
	for (int i = pos; i >= 0; i--) 
	{
		char ch = editor->text_buffer[i];
		char characters[] = " !@$%^&*<>`~|\'\"(){}[]=+-,./\\:;%\n\t";
		if (strchr(characters, ch) != NULL)
		{
			int size = pos - i + 1;

			// Deleting the line
			char* new_buffer = calloc(editor->buffer_len, sizeof(char));
			memcpy(new_buffer, editor->text_buffer, i);
			strcpy(new_buffer + i, editor->text_buffer + pos + 1);
			free(editor->text_buffer);
			editor->text_buffer = new_buffer;

			editor->buffer_len -= size;

			editor_set_cur_pos(editor, i);

			break;
		}
	}
}

void editor_delete_right(Editor* editor)
{
	int pos = editor_get_cur_pos(editor, editor->cur_x, editor->cur_y) + 1;
	for (int i = pos; i < editor->buffer_len; i++)
	{
		char ch = editor->text_buffer[i];
		char characters[] = " !@$%^&*<>`~|\'\"(){}[]=+-,./\\:;%\n\t";
		if (strchr(characters, ch) != NULL)
		{
			int size = i - pos + 1;

			// Deleting the line
			char* new_buffer = calloc(editor->buffer_len, sizeof(char));
			memcpy(new_buffer, editor->text_buffer, pos);
			strcpy(new_buffer + pos - 1, editor->text_buffer + i);
			free(editor->text_buffer);
			editor->text_buffer = new_buffer;

			editor->buffer_len -= size;
			break;
		}
	}
}

void editor_delete_sel(Editor* editor)
{
	editor_set_cur_pos(editor, editor->sel_start);

	int size = editor->sel_end - editor->sel_start;
	char* new_buffer = calloc(editor->buffer_len, sizeof(char));
	memcpy(new_buffer, editor->text_buffer, editor->sel_start + 1);
	strcpy(new_buffer + editor->sel_start, editor->text_buffer + editor->sel_end);
	free(editor->text_buffer);

	editor->text_buffer = new_buffer;
	editor->buffer_len -= size;
	if (editor->buffer_len < 0) editor->buffer_len = 0;
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
		editor->edited = true;
	}
}

void editor_cur_right(Editor* editor)
{
	int size = editor_line_len(editor, editor->cur_y + 1);
	char line[size];
	for (int i = 0; i < size; i++) line[i] = '\0';
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

void editor_jump_left(Editor* editor)
{
	int pos = editor_get_cur_pos(editor, editor->cur_x, editor->cur_y) - 1;
	for (int i = pos; i >= 0; i--) 
	{
		char ch = editor->text_buffer[i];
		char characters[] = " !@$%^&*<>`~|\'\"(){}[]=+-,./\\:;%\n\t";
		if (strchr(characters, ch) != NULL)
		{
			editor_set_cur_pos(editor, i - 1);
			break;
		}
	}
}

void editor_jump_right(Editor* editor)
{
	int pos = editor_get_cur_pos(editor, editor->cur_x, editor->cur_y) + 1;
	for (int i = pos; i < editor->buffer_len; i++)
	{
		char ch = editor->text_buffer[i];
		char characters[] = " !@$%^&*<>`~|\'\"(){}[]=+-,./\\:;%\n\t";
		if (strchr(characters, ch) != NULL)
		{
			editor_set_cur_pos(editor, i);
			break;
		}
	}
}

void editor_jump_up(Editor* editor)
{
	if (editor->cur_y - JUMP_SIZE + 1 > 0)
	{
		int size = editor_line_len(editor, editor->cur_y - JUMP_SIZE + 1);
		char line[size];
		editor_get_line(editor, editor->cur_y - JUMP_SIZE + 1, line);
		
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

 		editor->cur_y -= JUMP_SIZE;
 		editor->cur_rend_y -= JUMP_SIZE;

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

void editor_jump_down(Editor* editor)
{
	int line_no = editor_get_line_no(editor);
	if (editor->cur_y + JUMP_SIZE < line_no)
	{
		int size = editor_line_len(editor, editor->cur_y + 1 + JUMP_SIZE);
		char line[size];
		editor_get_line(editor, editor->cur_y + 1 + JUMP_SIZE, line);

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
		editor->cur_y += JUMP_SIZE;
		editor->cur_rend_y += JUMP_SIZE;

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

void editor_jump_top(Editor* editor)
{
	editor->cur_x = editor->cur_y = editor->cur_rend_x = editor->cur_rend_y = 0;
}

void editor_jump_bottom(Editor* editor)
{
	int lines = editor_get_line_no(editor);
	int size = editor_line_len(editor, lines);
	editor->cur_y = editor->cur_rend_y = lines - 1;
	editor->cur_x = editor->cur_rend_x = size;
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
	if (editor->cur_rend_x - editor->scroll_x >= (w - editor->cur_w * 6) / editor->cur_w)
	{
		int diff = ((w - editor->cur_w * 6)/ editor->cur_w) - (editor->cur_rend_x - editor->scroll_x) - 1;
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
	if (editor->cur_y - editor->scroll_y > (h - (3 * editor->cur_h))  / editor->cur_h)
	{
		int diff = ((h - (3 * editor->cur_h)) / editor->cur_h) - (editor->cur_y - editor->scroll_y);
		editor->scroll_y -= diff;
	}
}

// Clipboard handlings
void editor_copy(Editor* editor)
{
	int size = editor->sel_end - editor->sel_start + 1;
	char* selected = calloc(size, sizeof(char));
	memcpy(selected, editor->text_buffer + editor->sel_start, size);
	SDL_SetClipboardText(selected);
	free(selected);
}

void editor_paste(Editor* editor)
{
	if (SDL_HasClipboardText())
	{
		char* text = SDL_GetClipboardText();
		for (int i = 0; i < strlen(text); i++)
		{
			editor_insert(editor, text[i]);
		}
		SDL_free(text);
	}
}

// Editor selection
void editor_init_norm_select(Editor* editor)
{
	int pos = editor_get_cur_pos(editor, editor->cur_x, editor->cur_y);
	editor->sel_start = pos;
	editor->sel_end = pos;
	editor->sel_init = pos;

	editor->norm_visual = true;
	editor->line_visual = false;
}

void editor_init_line_select(Editor* editor)
{
	int pos = editor_get_cur_pos(editor, editor->cur_x, editor->cur_y);
	for (int i = pos; i <= editor->buffer_len; i++)
	{
		char ch = editor->text_buffer[i];
		if (ch == '\n')
		{
			editor->sel_end = i;
			break;
		}
		else if (i == editor->buffer_len)
		{
			editor->sel_end = i;
			break;
		}
	}

	for (int i = pos - 1; i >= 0; i--) 
	{
		char ch = editor->text_buffer[i];
		if (ch == '\n')
		{
			editor->sel_start = i + 1;
			break;
		}
		else if (i == 0)
		{
			editor->sel_start = i;
		}
	}
	// Initial line
	editor->sel_init = editor->cur_y;
	editor->norm_visual = false;
	editor->line_visual = true;
}

void editor_norm_select(Editor* editor)
{
	int pos = editor_get_cur_pos(editor, editor->cur_x, editor->cur_y);
	if (pos > editor->sel_init)
	{
		editor->sel_start = editor->sel_init;
		editor->sel_end = pos;
	}
	else if (pos < editor->sel_init)
	{
		editor->sel_start = pos;
		editor->sel_end = editor->sel_init;
	}
}

void editor_line_select(Editor* editor)
{
	int pos = editor_get_cur_pos(editor, editor->cur_x, editor->cur_y);
	if (editor->cur_y < editor->sel_init)
	{
		for (int i = pos; i >= 0; i--) 
		{
			char ch = editor->text_buffer[i];
			if (ch == '\n')
			{
				editor->sel_start = i + 1;
				break;
			}
			else if (i == 0)
			{
				editor->sel_start = 0;
			}
		}
	}
	else if (editor->cur_y > editor->sel_init)
	{
		for (int i = pos; i <= editor->buffer_len; i++)
		{
			char ch = editor->text_buffer[i];
			if (ch == '\n')
			{
				editor->sel_end = i;
				break;
			}
			else if (i == editor->buffer_len)
			{
				editor->sel_end = i;
				break;
			}
		}
	}
	else if (editor->cur_y == editor->sel_init)
	{
		for (int i = pos; i <= editor->buffer_len; i++)
		{
			char ch = editor->text_buffer[i];
			if (ch == '\n')
			{
				editor->sel_end = i;
				break;
			}
			else if (i == editor->buffer_len)
			{
				editor->sel_end = i;
				break;
			}
		}

		for (int i = pos - 1; i >= 0; i--) 
		{
			char ch = editor->text_buffer[i];
			if (ch == '\n')
			{
				editor->sel_start = i + 1;
				break;
			}
			else if (i == 0)
			{
				editor->sel_start = i;
			}
		}
	}
}

// Editor rendering
void editor_render_select(Editor* editor, Colors* colors_rgb)
{
	// Rendering the selection line 
	int x = 0, y = 0, w = editor->cur_w;
	for (int i = 0; i < editor->buffer_len; i++)
	{
		w = editor->cur_w;

		if (i >= editor->sel_start && i <= editor->sel_end)
		{
			if (editor->text_buffer[i] == '\t')
			{
				w = (TAB_SIZE) * editor->cur_w;
			}

			SDL_Rect pos = { (x - editor->scroll_x) * editor->cur_w, (y - editor->scroll_y) * editor->cur_h, w, editor->cur_h };
			SDL_SetRenderDrawColor(editor->window->renderer, colors_rgb->selection.r, colors_rgb->selection.g, colors_rgb->selection.b, colors_rgb->selection.a);
			SDL_RenderFillRect(editor->window->renderer, &pos);
		}

		if (editor->text_buffer[i] == '\n')
		{
			y++;
			x = 0;
		}
		else if (editor->text_buffer[i] == '\t')
			x += TAB_SIZE;
		else
			x++;
	}
}

void editor_render_buffer(Editor* editor, int start, int end, TTF_Font* font, Colors* colors_rgb)
{
	// Setting up the render target for text buffer
	SDL_SetRenderTarget(editor->window->renderer, editor->editor_texture);
	SDL_SetRenderDrawColor(editor->window->renderer, colors_rgb->editor_bg.r, colors_rgb->editor_bg.g, colors_rgb->editor_bg.b, colors_rgb->editor_bg.a);
	SDL_RenderClear(editor->window->renderer);

	// Selection rendering and updating
	if (editor->window->mode == VISUAL)
	{
		if (editor->norm_visual)
			editor_norm_select(editor);
		else if (editor->line_visual)
			editor_line_select(editor);
		editor_render_select(editor, colors_rgb);
	}

	// Flushing the buffer into the render target
	if (editor->buffer_len > 0)
	{
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
						for (int i = 0; i < TAB_SIZE; i++) text[k+i] = ' ';
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

				if (strlen(text) > 0)
				{
					int x = 0, w = 0, h = 0;
					if (editor->syntax_on)
					{
						// Parsing the text according to the spaces and special characters
						int p = 0;

						// Flags
						bool comment = false;
						bool str_start = false;
						bool str_end = false;
						
						while (p < buff_size)
						{
							int space = 0;
							
							char spc[1];
							char token[buff_size];
							token[0] = '\0';
							for (int i = p; i < buff_size; i++)
							{
								char ch = text[i];
								if (ch != ' ' && !strchr(special_char, ch))
								{
									char str[1];
									sprintf(str, "%c", ch);
									strcat(token, str);
									strcpy(spc, "\0");
									p++;
								}
								else if (ch == ' ')
								{
									space++;
									p++;
									strcpy(spc, "\0");
									break;
								}
								else if (strchr(special_char, ch) != NULL)
								{
									sprintf(spc, "%c", ch);
									p++;
									break;
								}
							}

							if (strlen(token) > 0)
							{
								SDL_Texture* texture = create_texture(editor->window->renderer, font, token);
								SDL_QueryTexture(texture, NULL, NULL, &w, &h);
								SDL_Rect pos = { x - (editor->scroll_x * editor->cur_w) , y * h, w, h };

								bool matched = false;
								if ((!str_start && !str_end) && !comment)
								{
									// Checking for data types
									if (!matched)
									{
										for (int i = 0; i < editor->data_types_len; i++)
										{
											if (!strcmp(token, editor->data_types[i]))
											{
												SDL_SetTextureColorMod(texture, colors_rgb->types.r, colors_rgb->types.g, colors_rgb->types.b);
												matched = true;
												break;
											}	
										}
									}

									// Checking for keywords
									if (!matched)
									{
										for (int i = 0; i < editor->keywords_len; i++)
										{
											if (!strcmp(token, editor->keywords[i]))
											{
												SDL_SetTextureColorMod(texture, colors_rgb->keywords.r, colors_rgb->keywords.g, colors_rgb->keywords.b);
												matched = true;
												break;
											}
										}
									}

									// Checking for operating statements
									if (!matched)
									{
										for (int i = 0; i < editor->op_statements_len; i++)
										{
											if (!strcmp(token, editor->op_statements[i]))
											{
												SDL_SetTextureColorMod(texture, colors_rgb->op_statements.r, colors_rgb->op_statements.g, colors_rgb->op_statements.b);
												matched = true;
												break;
											}
										}
									}

									// Checking for functions
									if (!matched)
									{
										for (int i = 0; i < editor->functions_len; i++)
										{
											if (!strcmp(token, editor->functions[i]))
											{
												SDL_SetTextureColorMod(texture, colors_rgb->functions.r, colors_rgb->functions.g, colors_rgb->functions.b);
												matched = true;
												break;
											}
										}
									}

									// Checking for constants or numbers
									if (!matched)
									{
										if (is_upper(token) || is_no(token))
										{
											SDL_SetTextureColorMod(texture, colors_rgb->constants.r, colors_rgb->constants.g, colors_rgb->constants.b);
											matched = true;
										}
										else 
										{
											if (editor->bools != NULL)
											{
												for (int i = 0; i < 2; i++)
												{
													if (!strcmp(token, editor->bools[i]))
													{
														SDL_SetTextureColorMod(texture, colors_rgb->constants.r, colors_rgb->constants.g, colors_rgb->constants.b);
														matched = true;
														break;
													}
												}
											}
										}
									}
								}

								// For string
								if (!matched && ((str_start && !str_end) || (!str_start && str_end)))
								{
									SDL_SetTextureColorMod(texture, colors_rgb->string.r, colors_rgb->string.g, colors_rgb->string.b);
									matched = true;
								}
								// For comment
								else if (comment && !matched)
								{
									SDL_SetTextureColorMod(texture, colors_rgb->comment.r, colors_rgb->comment.g, colors_rgb->comment.b);
									matched = true;
								}
								else if (!matched)
									SDL_SetTextureColorMod(texture, colors_rgb->editor_fg.r, colors_rgb->editor_fg.g, colors_rgb->editor_fg.b);

								SDL_RenderCopy(editor->window->renderer, texture, NULL, &pos);
								SDL_DestroyTexture(texture);
								x += w;
							}
							if (strlen(spc) > 0)
							{
								SDL_Texture* texture = create_texture(editor->window->renderer, font, spc);
								SDL_QueryTexture(texture, NULL, NULL, &w, &h);
								SDL_Rect pos = { x - (editor->scroll_x * editor->cur_w), y * h, w, h };

								// Checking for string
								if (!strcmp(spc, "\"") || !strcmp(spc, "\'"))
								{
									if (str_start)
									{
										str_start = false;
										str_end = true;
									}
									else
									{
										str_start = true;
										str_end = false;
									}
									// Adding support for slash quotes for strings
									if (p > 1)
									{
										if (text[p - 2] == '\\')
										{
											str_start = true;
											str_end = false;
										}
									}
								}
								// Checking for comments
								else if ((!strcmp(spc, "#")) || ((!strcmp(spc, "/") && text[p] == '/')))
								{
									comment = true;
								}

								if (!strcmp(spc, "<") && comment)
								{
									str_start = true;
									str_end = false;
								}
								else if (!strcmp(spc, ">") && comment)
								{
									str_start = false;
									str_end = true;
								}
								
								if ((!str_start && !str_end) && !comment)
									SDL_SetTextureColorMod(texture, colors_rgb->symbols.r, colors_rgb->symbols.g, colors_rgb->symbols.b);

								if ((str_start && !str_end) || (!str_start && str_end))
								{
									SDL_SetTextureColorMod(texture, colors_rgb->string.r, colors_rgb->string.g, colors_rgb->string.b);
									if (str_end) str_end = false;
								}
								else if (comment)
								{
									SDL_SetTextureColorMod(texture, colors_rgb->comment.r, colors_rgb->comment.g, colors_rgb->comment.b);
								}

								SDL_RenderCopy(editor->window->renderer, texture, NULL, &pos);
								SDL_DestroyTexture(texture);
								x += editor->cur_w;
							}
							else if (space > 0)
								x += editor->cur_w;
						}
					}
					else
					{
						// Plane rendering
						SDL_Texture* texture = create_texture(editor->window->renderer, font, text);
						SDL_QueryTexture(texture, NULL, NULL, &w, &h);
						editor_draw_line(editor, -editor->scroll_x, y, texture, colors_rgb->editor_fg);
						SDL_DestroyTexture(texture);
					}
				}
			}
		}
	}
	// Rendering cursor
	SDL_Rect cur_rect = { (editor->cur_rend_x - editor->scroll_x) * editor->cur_w,  (editor->cur_rend_y - editor->scroll_y) * editor->cur_h , editor->cur_w, editor->cur_h };
	SDL_SetRenderDrawColor(editor->window->renderer, colors_rgb->cursor.r, colors_rgb->cursor.g, colors_rgb->cursor.b, colors_rgb->cursor.a);
	SDL_RenderFillRect(editor->window->renderer, &cur_rect); 

	// Rendering the highlighted character
	if (editor->buffer_len > 0)
	{
		int pos = editor_get_cur_pos(editor, editor->cur_x, editor->cur_y);
		char ch = editor->text_buffer[pos];
		if (ch != '\n' && ch != '\t' && ch)
		{
			char c[1];
			sprintf(c, "%c", ch);
			SDL_Texture* texture = create_texture(editor->window->renderer, font, c);
			draw_text(editor->window->renderer, editor->cur_rend_x - editor->scroll_x, editor->cur_rend_y - editor->scroll_y, texture, colors_rgb->editor_bg);
			SDL_DestroyTexture(texture);
		}
	}
}

void editor_render_line(Editor* editor, int start, int end, TTF_Font* font, Colors* colors_rgb)
{
	// Changing the target texture
	SDL_SetRenderTarget(editor->window->renderer, editor->line_texture);
	SDL_SetRenderDrawColor(editor->window->renderer, colors_rgb->line_bg.r, colors_rgb->line_bg.g, colors_rgb->line_bg.b, colors_rgb->line_bg.a);
	SDL_RenderClear(editor->window->renderer);

	for (int i = start; i < end; i++) 
	{
		char c[end-start + 1];
		sprintf(c, "%d\0", i);
		SDL_Texture* texture = sdl_check_ptr(create_texture(editor->window->renderer, font, c));

		// Changing color if the line has the cursor
		if (i - 1 == editor->cur_rend_y)
			draw_text(editor->window->renderer, 0, i - 1 - editor->scroll_y, texture, colors_rgb->cur_line_fg);
		else
			draw_text(editor->window->renderer, 0, i - 1 - editor->scroll_y, texture, colors_rgb->line_fg);

	    SDL_DestroyTexture(texture);
	}
}

void editor_render_bar(Editor* editor, TTF_Font* font, Colors* colors_rgb)
{
	// Setting up the render target for the command buffer
	SDL_SetRenderTarget(editor->window->renderer, editor->bar_texture);
	SDL_SetRenderDrawColor(editor->window->renderer, colors_rgb->bar_bg.r, colors_rgb->bar_bg.g, colors_rgb->bar_bg.b, colors_rgb->bar_bg.a);
	SDL_RenderClear(editor->window->renderer);

    int w, h; 
	
	// Rendering mode
	char mode[3];
	sprintf(mode, " %c ", editor->window->mode);
	SDL_Texture* char_texture = create_texture(editor->window->renderer, font, mode);

	SDL_QueryTexture(char_texture, NULL, NULL, &w, &h);
	SDL_Rect pos_0 = { 0, 0, w, h };

	// Setting color according the the current mode
	SDL_Color mode_color;
	if (editor->window->mode == NORMAL)
		mode_color = colors_rgb->normal_md;
	if (editor->window->mode == INSERT)
		mode_color = colors_rgb->insert_md;
	if (editor->window->mode == VISUAL)
		mode_color = colors_rgb->visual_md;
	if (editor->window->mode == COMMAND)
		mode_color = colors_rgb->command_md;

	SDL_SetRenderDrawColor(editor->window->renderer, mode_color.r, mode_color.g, mode_color.b, mode_color.a);
	SDL_RenderFillRect(editor->window->renderer, &pos_0);

	draw_text(editor->window->renderer, 0, 0, char_texture, colors_rgb->bar_fg);
   	SDL_DestroyTexture(char_texture);	

	// Rendering file name
	if (strlen(editor->file_name) > 0)
	{
		SDL_Texture* file_name = create_texture(editor->window->renderer, font, editor->file_name);
		SDL_Rect pos_1 = { w, 0, w, h };
		SDL_QueryTexture(file_name, NULL, NULL, &w, &h);
		pos_1.w = w;
		pos_1.h = h;
		SDL_RenderCopy(editor->window->renderer, file_name, NULL, &pos_1);
		SDL_DestroyTexture(file_name);
	}

	// Rendering the cursor position
	char cur_pos[5+1+5] = {0};
	sprintf(cur_pos, "%d:%d", editor->cur_x, editor->cur_y);

	SDL_Texture* cur_pos_texture = create_texture(editor->window->renderer, font, cur_pos);
	SDL_QueryTexture(cur_pos_texture, NULL, NULL, &w, &h);
	SDL_Rect pos = { editor->window->width - w - 5, 0, w, h };
	SDL_RenderCopy(editor->window->renderer, cur_pos_texture, NULL, &pos);
	SDL_DestroyTexture(cur_pos_texture);

	// Edited indicator
	if (editor->edited)
	{
		char sym[2] = "+\0";
		SDL_Texture* sym_texture = create_texture(editor->window->renderer, font, sym);
		SDL_Rect pos = { editor->window->width - w - 10, 0, w, h };
		SDL_QueryTexture(sym_texture, NULL, NULL, &w, &h);
		pos.x -= w;
		pos.w = w;
		pos.h = h;
		SDL_RenderCopy(editor->window->renderer, sym_texture, NULL, &pos);
		SDL_DestroyTexture(sym_texture);
	}
}

void editor_render(Editor* editor, Colors* colors_rgb)
{
	// Calling scrolling functions
	editor_scroll_left (editor);
	editor_scroll_right(editor);
	editor_scroll_up   (editor);
	editor_scroll_down (editor);

	// Calculating the visible lines
	int total_line = editor_get_line_no(editor);
	int start = editor->scroll_y + 1;
	int end = editor->window->height / editor->cur_h + editor->scroll_y - 1;
	if (end > total_line) end = total_line + 1;

	// Rendering buffers
	editor_render_line(editor, start, end, editor->font_1, colors_rgb);
	editor_render_buffer(editor, start, end, editor->font_1, colors_rgb);
	editor_render_bar(editor, editor->font_2, colors_rgb);

	// Defaulting the render target
	SDL_SetRenderTarget(editor->window->renderer, NULL);

	// Rendering text buffer
	SDL_Rect texture_rect = { editor->cur_w * 6, 0, editor->window->width, editor->window->height};
	SDL_RenderCopy(editor->window->renderer, editor->editor_texture, NULL, &texture_rect);
	
	// Rendering line buffer
	SDL_Rect line_rect = { 0, 0, editor->cur_w * 6, editor->window->height};
	SDL_RenderCopy(editor->window->renderer, editor->line_texture, NULL, &line_rect);
	
	// Rendering bar line buffer
	SDL_Texture* ch = create_texture(editor->window->renderer, editor->font_2, "A");
	int w, h;
	SDL_QueryTexture(ch, NULL, NULL, &w, &h);
	SDL_DestroyTexture(ch);

	SDL_Rect cmd_line_rect = { 0, editor->window->height - (2 * h), editor->window->width, h }; 
	SDL_RenderCopy(editor->window->renderer, editor->bar_texture, NULL, &cmd_line_rect);
}

void editor_gen_texture(Editor* editor, SDL_Renderer* renderer, TTF_Font* font)
{
	SDL_Texture* ch = create_texture(renderer, font, "A");
	SDL_QueryTexture(ch, NULL, NULL, &editor->cur_w, &editor->cur_h);
	SDL_DestroyTexture(ch);

	// Generating textures for the buffers
	editor->editor_texture = sdl_check_ptr(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, editor->window->width , editor->window->height));
	editor->line_texture = sdl_check_ptr(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, editor->cur_w * 6, editor->window->height));

	ch = create_texture(editor->window->renderer, editor->font_2, "A");
	int w, h;
	SDL_QueryTexture(ch, NULL, NULL, &w, &h);
	SDL_DestroyTexture(ch);

	editor->bar_texture = sdl_check_ptr(SDL_CreateTexture(editor->window->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, editor->window->width, h));
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

