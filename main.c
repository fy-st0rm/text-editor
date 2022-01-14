#include "globals.h"
#include "editor.h"
#include "window.h"
#include "cmd_line.h"

/*
 * TODO: [ ] Command line
 * TODO: [ ] Vim like modes (NORMAL, INSERT, COMMAND, VISUAL)
 * TODO: [ ] Colors
 * TODO: [ ] Fix the memory leak while resizing the window
 * TODO: [ ] Read and write to the file
 * TODO: [ ] Clipboard handling
 * TODO: [ ] Text selection
 * TODO: [X] Proper cursor movement
 * TODO: [X] Deal with tabs
 * TODO: [X] Render only visible characters
 * TODO: [X] Fix the corrupted characters
 * TODO: [X] Line buffer and command buffer
 * TODO: [X] There is a memory leak where we use editor_get_line() function
 * TODO: [X] Text insert with cursor position 
 * TODO: [X] Text deletion 
 * TODO: [X] Scrolling
 * TODO: [X] Cursor movement
 * TODO: [X] Fix a bug while inserting between the line
 */

void arrow_movement(Editor* editor, SDL_Event event)
{
	switch (event.key.keysym.sym)
	{
		// Cursor movements
		case SDLK_LEFT:
			editor_cur_left(editor);
			break;

		case SDLK_RIGHT:
			editor_cur_right(editor);
			break;

		case SDLK_UP:
			editor_cur_up(editor);
			break;

		case SDLK_DOWN:
			editor_cur_down(editor);
			break;
	}
}

void hjkl_movement(Editor* editor, SDL_Event event)
{
	switch (event.key.keysym.sym)
	{
		// Cursor movements
		case SDLK_h:
			editor_cur_left(editor);
			break;

		case SDLK_l:
			editor_cur_right(editor);
			break;

		case SDLK_k:
			editor_cur_up(editor);
			break;

		case SDLK_j:
			editor_cur_down(editor);
			break;
	}
}

int main(int argc, char** argv)
{
	sdl_check(SDL_Init(SDL_INIT_VIDEO));
	sdl_check(TTF_Init());

	atexit(report_mem_leak);
	
	init_colors();

	// Loading font
	char* font_path = "JetBrainsMonoNL-Regular.ttf";
	int font_size = 16;
	TTF_Font* font = sdl_check_ptr(TTF_OpenFont(font_path, font_size));

	Window* window = window_new("Text Editor", 800, 600);
	Cmd_line* cmd_line = cmd_line_new(window, font);


	// List of buffers
	int curr_buffer = 0;
	Editor* buffers[1];
	Editor* editor = editor_new(window, font, "");
	buffers[0] = editor;


	// Flags
	bool loop = true;
	bool halt = false;
	SDL_Event event;

	while (loop)
	{
		if (SDL_WaitEvent(&event))
		{
			if (event.type == SDL_QUIT)
				loop = false;
			else if (event.type == SDL_TEXTINPUT)
			{
				if (window->mode == INSERT)
				{
					if (!halt)
					{
						char* text = event.text.text;
						editor_insert(buffers[curr_buffer], text[0]);
					}
					else
					{
						halt = false;
					}
				}
				else if (window->mode == COMMAND)
				{
					if (!halt)
					{
						char* text = event.text.text;
						cmd_line_insert(cmd_line, text[0]);
					}
					else
					{
						halt = false;
					}
				}
			}
			else if (event.type == SDL_KEYDOWN)
			{
				// INSERT MODE SPECIFIC
				if (window->mode == INSERT)
				{
					switch (event.key.keysym.sym)
					{
						case SDLK_RETURN:
							editor_insert(buffers[curr_buffer], '\n');
							break;
						case SDLK_TAB:
							editor_insert(buffers[curr_buffer], '\t');
							break;
						case SDLK_BACKSPACE:
							editor_backspace(buffers[curr_buffer]);
							break;

						// Mode switcher
						case SDLK_ESCAPE:
							window->mode = NORMAL;
							break;
					}
					arrow_movement(buffers[curr_buffer], event);
				}
				// NORMAL MODE SPECIFIC
				else if (window->mode == NORMAL)
				{
					switch (event.key.keysym.sym)
					{
						// Mode switcher
						case SDLK_i:
							window->mode = INSERT;
							halt = true;
							break;
						
						case SDLK_SEMICOLON:
							window->mode = COMMAND;
							cmd_line_set_prompt(cmd_line, ":");
							cmd_line_clear_input(cmd_line);
							halt = true;
							break;
					}
					hjkl_movement(buffers[curr_buffer], event);
					arrow_movement(buffers[curr_buffer], event);
				}
				// COMMAND MODE SPECIFIC
				else if (window->mode == COMMAND)
				{
					switch (event.key.keysym.sym)
					{
						case SDLK_RETURN:
							cmd_line_parse(cmd_line, buffers, &curr_buffer);
							cmd_line_clear_prompt(cmd_line);
							window->mode = NORMAL;
							break;

						case SDLK_BACKSPACE:
							cmd_line_backspace(cmd_line);
							break;

						// Command line movement
						case SDLK_LEFT:
							cmd_line_cur_left(cmd_line);
							break;
						case SDLK_RIGHT:
							cmd_line_cur_right(cmd_line);
							break;


						// Mode switcher
						case SDLK_ESCAPE:
							window->mode = NORMAL;
							cmd_line_clear_prompt(cmd_line);
							cmd_line_clear_input(cmd_line);
							break;
					}
				}
			}
			else if (event.type == SDL_WINDOWEVENT)
			{
				switch (event.window.event)
				{
					case SDL_WINDOWEVENT_RESIZED:
						SDL_GetWindowSize(window->window, &window->width, &window->height);
						for (int i = 0; i < sizeof(buffers) / sizeof(buffers[0]); i++)
							editor_resize(buffers[i]);
						cmd_line_resize(cmd_line);
						break;
				}
			}

			if (curr_buffer < 0)
			{
				loop = false;
			}
			else
			{
				window_clear(window, colors_rgb->editor_bg); 
				
				for (int i = 0; i < sizeof(buffers) / sizeof(buffers[0]); i++)
					editor_render(editor, window, font, colors_rgb);

				cmd_line_render(cmd_line, font, colors_rgb);
			}
		}
	}

	free_colors();
	cmd_line_destroy(cmd_line);
	window_destroy(window);

	return 0;
}
