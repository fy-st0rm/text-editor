#include "cmd_line.h"


Cmd_line* cmd_line_new(Window* window, TTF_Font* font)
{
	Cmd_line* cmd_line = malloc(sizeof(Cmd_line));
	
	cmd_line->window = window;

	// Character buffers
	cmd_line->input = calloc(MAX_INPUT, sizeof(char));
	cmd_line->prompt = calloc(MAX_INPUT, sizeof(char));

	cmd_line->cur_x = 0;
	cmd_line->cur_y = 0;
	SDL_Texture* ch_text = create_texture(window->renderer, font, "A");
	SDL_QueryTexture(ch_text, NULL, NULL, &cmd_line->cur_w, &cmd_line->cur_h);
	SDL_DestroyTexture(ch_text);
	cmd_line->scroll_x = 0;

	// Main texture
	cmd_line->texture = sdl_check_ptr(SDL_CreateTexture(window->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, window->width, cmd_line->cur_h));

	return cmd_line;
}

void cmd_line_destroy(Cmd_line* cmd_line)
{
	free(cmd_line->input);
	free(cmd_line->prompt);
	SDL_DestroyTexture(cmd_line->texture);
	free(cmd_line);
}

void cmd_line_resize(Cmd_line* cmd_line)
{
	SDL_DestroyTexture(cmd_line->texture);
	cmd_line->texture = sdl_check_ptr(SDL_CreateTexture(cmd_line->window->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, cmd_line->window->width, cmd_line->cur_h));
}

void cmd_line_insert(Cmd_line* cmd_line, char ch)
{
	if (strlen(cmd_line->input) < MAX_INPUT)
	{
		if (cmd_line->cur_x == strlen(cmd_line->input) && cmd_line->cur_x < MAX_INPUT)
		{
			cmd_line->input[cmd_line->cur_x] = ch;
		}
		else
		{
			char* new_input = calloc(MAX_INPUT, sizeof(char));
			memmove(new_input, cmd_line->input, cmd_line->cur_x);
			char c[1] = {0};
			sprintf(c, "%c", ch);
			strcpy(new_input + cmd_line->cur_x, c);
			memcpy(new_input + cmd_line->cur_x + 1, cmd_line->input + cmd_line->cur_x, strlen(cmd_line->input) - cmd_line->cur_x);

			free(cmd_line->input);
			cmd_line->input = new_input;
		}
		cmd_line->cur_x++;
	}
}

void cmd_line_backspace(Cmd_line* cmd_line)
{
	if (cmd_line->cur_x > 0)
	{
		cmd_line->cur_x--;
		memmove(&cmd_line->input[cmd_line->cur_x], &cmd_line->input[cmd_line->cur_x + 1], strlen(cmd_line->input) - cmd_line->cur_x);
	}
}

void cmd_line_quit_with_save(Cmd_line* cmd_line, Editor** buffers, int* curr_buffer, int* buffer_amt)
{
	if (!buffers[*curr_buffer]->edited)
	{
		editor_destroy(buffers[*curr_buffer]);
		--*buffer_amt;
		if (*buffer_amt > 0)
		{
			memmove(buffers[*curr_buffer], buffers[*curr_buffer + 1], buffer_amt + *curr_buffer);
			--*curr_buffer;
		}
		else
			*curr_buffer = -1;
	}
	else
	{
		char* reply = replies[4];
		cmd_line_clear_input(cmd_line);
		for (int i = 0; i < strlen(reply); i++)
		{
			cmd_line_insert(cmd_line, reply[i]);
		}
	}
}

void cmd_line_quit_without_save (Cmd_line* cmd_line, Editor** buffers, int* curr_buffer, int* buffer_amt)
{
	editor_destroy(buffers[*curr_buffer]);
	--*buffer_amt;
	if (*buffer_amt > 0)
	{
		memmove(buffers[*curr_buffer], buffers[*curr_buffer + 1], buffer_amt + *curr_buffer);
		--*curr_buffer;
	}
	else
		*curr_buffer = -1;
}

int cmd_line_save(Cmd_line* cmd_line, char* file_name, Editor** buffers, int* curr_buffer)
{
	int result = editor_write_file(buffers[*curr_buffer], file_name);

	// Writing the msg log
	char* reply = replies[result];
	cmd_line_clear_input(cmd_line);
	for (int i = 0; i < strlen(reply); i++)
	{
		cmd_line_insert(cmd_line, reply[i]);
	}
	return result;
}

void cmd_line_read(Cmd_line* cmd_line, char* file_name, Editor** buffers, int* curr_buffer)
{
	int result = editor_read_file(buffers[*curr_buffer], file_name);
	
	// Writing the msg log
	char* reply = replies[result];
	cmd_line_clear_input(cmd_line);
	for (int i = 0; i < strlen(reply); i++)
	{
		cmd_line_insert(cmd_line, reply[i]);
	}
}

void cmd_line_parse(Cmd_line* cmd_line, Editor** buffers, int* curr_buffer, int* buffer_amt)
{
	int size = 0;
	int len = strlen(cmd_line->input);
	if (len != 0)
	   	size++;
	else
		return;

	for (int i = 0; i < len;i++)
		if (cmd_line->input[i] == ' ') size++;


	char cmds[size][MAX_INPUT];

	// Splitting the cmds
	int p = 0;
	char temp[len];
	strcpy(temp, cmd_line->input);
	char* token = strtok(temp, " ");
	strcpy(cmds[p], token);

	while (token != NULL)
	{
		p++;
		token = strtok(NULL, " ");
		if (token != NULL && p < size)
			strcpy(cmds[p], token);
	}

	for (int i = 0; i < size; i++)
	{
		// Quiting the buffer
		if (!strcmp(cmds[i], "q"))
		{
			cmd_line_quit_with_save(cmd_line, buffers, curr_buffer, buffer_amt);
			break;
		}
		// Quiting without saving the changes
		else if (!strcmp(cmds[i], "q!"))
		{
			cmd_line_quit_without_save(cmd_line, buffers, curr_buffer, buffer_amt);
			break;
		}
		// Saving the files
		else if (!strcmp(cmds[i], "w"))
		{
			char file_name[256];
			if (size > i + 1)
				strcpy(file_name, cmds[++i]);
			else
				strcpy(file_name, buffers[*curr_buffer]->file_name);
			
			cmd_line_save(cmd_line, file_name, buffers, curr_buffer);
		}
		// Reading the files
		else if (!strcmp(cmds[i], "e"))
		{
			char file_name[256];
			for (int i = 0; i < 256; i++) file_name[i] = '\0';

			if (size > i + 1)
				strcpy(file_name, cmds[++i]);
			
			cmd_line_read(cmd_line, file_name, buffers, curr_buffer);
		}
		// Save and quit
		else if (!strcmp(cmds[i], "wq"))
		{
			char file_name[256];
			if (size > i + 1)
				strcpy(file_name, cmds[++i]);
			else
				strcpy(file_name, buffers[*curr_buffer]->file_name);

			int result = cmd_line_save(cmd_line, file_name, buffers, curr_buffer);
			if (result == 0)
				cmd_line_quit_with_save(cmd_line, buffers, curr_buffer, buffer_amt);
		}
	}
}

void cmd_line_set_prompt(Cmd_line* cmd_line, char* new_prompt)
{
	strcpy(cmd_line->prompt, new_prompt);
}

void cmd_line_clear_prompt(Cmd_line* cmd_line)
{
	for (int i = 0; i < MAX_INPUT; i++) cmd_line->prompt[i] = '\0';
}

void cmd_line_clear_input(Cmd_line* cmd_line)
{
	for (int i = 0; i < MAX_INPUT; i++) cmd_line->input[i] = '\0';
	cmd_line->cur_x = 0;
}

void cmd_line_cur_left(Cmd_line* cmd_line)
{
	if (cmd_line->cur_x > 0)
		cmd_line->cur_x--;
}

void cmd_line_cur_right(Cmd_line* cmd_line)
{
	if (cmd_line->cur_x < strlen(cmd_line->input))
		cmd_line->cur_x++;
}

void cmd_line_render(Cmd_line* cmd_line, TTF_Font* font, Colors* colors_rgb)
{
	// Rendering
	SDL_SetRenderTarget(cmd_line->window->renderer, cmd_line->texture);
	SDL_SetRenderDrawColor(cmd_line->window->renderer, colors_rgb->editor_bg.r, colors_rgb->editor_bg.g, colors_rgb->editor_bg.b, colors_rgb->editor_bg.a);
	SDL_RenderClear(cmd_line->window->renderer);

	int w = 0;
	int h = 0;

	// Rendering the prompt
	if (strlen(cmd_line->prompt) > 0)
	{
		SDL_Texture* prompt_texture = create_texture(cmd_line->window->renderer, font, cmd_line->prompt);
		SDL_QueryTexture(prompt_texture, NULL, NULL, &w, &h);
		SDL_Rect pos_0 = { 0, 0, w, h }; 
		SDL_RenderCopy(cmd_line->window->renderer, prompt_texture, NULL, &pos_0);
		SDL_DestroyTexture(prompt_texture);
	}

	// Rendering the input
	if (strlen(cmd_line->input) > 0)
	{
		SDL_Texture* input_texture = create_texture(cmd_line->window->renderer, font, cmd_line->input);

		SDL_Rect pos_1 = { w, 0, 0, cmd_line->cur_h };
		SDL_QueryTexture(input_texture, NULL, NULL, &w, &h);
		pos_1.w = w;

		SDL_RenderCopy(cmd_line->window->renderer, input_texture, NULL, &pos_1);
		SDL_DestroyTexture(input_texture);
	}

	// Rendering cursor
	if (cmd_line->window->mode == COMMAND)
	{
		SDL_Rect cur_rect = { (cmd_line->cur_x + strlen(cmd_line->prompt)) * cmd_line->cur_w, cmd_line->cur_y, cmd_line->cur_w, cmd_line->cur_h };
		SDL_SetRenderDrawColor(cmd_line->window->renderer, colors_rgb->cursor.r, colors_rgb->cursor.g, colors_rgb->cursor.b, colors_rgb->cursor.a);
		SDL_RenderFillRect(cmd_line->window->renderer, &cur_rect); 
	}

	SDL_SetRenderTarget(cmd_line->window->renderer, NULL);
	SDL_Rect rect = { 0, cmd_line->window->height - cmd_line->cur_h, cmd_line->window->width, cmd_line->cur_h };
	SDL_RenderCopy(cmd_line->window->renderer, cmd_line->texture, NULL, &rect);
}
