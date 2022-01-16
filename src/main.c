#include "globals.h"
#include "settings.h"
#include "editor.h"
#include "window.h"
#include "cmd_line.h"

/*
 * TODO: [ ] Config interpretor
 * TODO: [ ] Colors
 * TODO: [ ] Undo and redo
 * TODO: [ ] Fix the memory leak while resizing the window
 * TODO: [X] Auto indent
 * TODO: [X] Multiple buffers
 * TODO: [X] Font scaling
 * TODO: [X] Text selection
 * TODO: [X] Vim like modes ([X]NORMAL, [X]INSERT, [X]COMMAND, [X]VISUAL)
 * TODO: [X] Cut, delete selected, replace selected
 * TODO: [X] Clipboard handling
 * TODO: [X] Command line
 * TODO: [X] Read and write to the file
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

// Keys for key bindings
typedef struct 
{
	bool ctrl;
	bool shift;
	bool d;
	bool g;
} Keys;

Keys* init_keys()
{
	Keys* keys = calloc(1, sizeof(Keys));

	keys->ctrl = false;
	keys->shift = false;
	keys->d = false;
	keys->g = false;
}

void arrow_movement(Editor* editor, SDL_Event event, Keys* keys, int* curr_buffer, int buffer_amt)
{
	switch (event.key.keysym.sym)
	{
		// Cursor movements
		case SDLK_LEFT:
			if (keys->ctrl)
			{
				if (*curr_buffer - 1 >= 0)
				{
					--*curr_buffer;
				}
			}
			else
				editor_cur_left(editor);
			break;

		case SDLK_RIGHT:
			if (keys->ctrl)
			{
				if (*curr_buffer + 1 < buffer_amt)
				{
					++*curr_buffer;
				}
			}
			else
				editor_cur_right(editor);
			break;

		case SDLK_UP:
			if (keys->ctrl)
				editor_jump_up(editor);
			else
				editor_cur_up(editor);
			break;

		case SDLK_DOWN:
			if (keys->ctrl)
				editor_jump_down(editor);
			else
				editor_cur_down(editor);
			break;
	}
}

void hjkl_movement(Editor* editor, SDL_Event event, Keys* keys, int* curr_buffer, int buffer_amt, Cmd_line* cmd_line)
{
	switch (event.key.keysym.sym)
	{
		// Cursor movements
		case SDLK_h:
			if (keys->ctrl)
			{
				if (*curr_buffer - 1 >= 0)
				{
					--*curr_buffer;
				}
			}
			else
				editor_cur_left(editor);
			break;

		case SDLK_l:
			if (keys->ctrl)
			{
				if (*curr_buffer + 1 < buffer_amt)
				{
					++*curr_buffer;
				}
			}
			else
				editor_cur_right(editor);
			break;

		case SDLK_k:
			editor_cur_up(editor);
			break;

		case SDLK_j:
			editor_cur_down(editor);
			break;

		case SDLK_w:
			if (!keys->d)
				editor_jump_right(editor);
			else if (keys->d)
			{
				if (editor->modifiable)
				{
					editor_delete_right(editor);
				}
				else
				{
					cmd_line_clear_input(cmd_line);
					char* reply = replies[5];
					for (int i = 0; i < strlen(reply); i++)
					{
						cmd_line_insert(cmd_line, reply[i]);
					}
				}
				keys->d = false;
			}
			break;
		
		case SDLK_b:
			if (!keys->d)
				editor_jump_left(editor);
			else if (keys->d)
			{
				if (editor->modifiable) 
				{
					editor_delete_left(editor);
				}
				else
				{
					cmd_line_clear_input(cmd_line);
					char* reply = replies[5];
					for (int i = 0; i < strlen(reply); i++)
					{
						cmd_line_insert(cmd_line, reply[i]);
					}
				}
				keys->d = false;
			}
			break;

		case SDLK_u:
			if (keys->ctrl)
				editor_jump_up(editor);
			break;

		case SDLK_d:
			if (keys->ctrl)
				editor_jump_down(editor);
			else if (keys->d)
			{
				if (editor->modifiable)
				{
					editor_delete_line(editor);
				}
				else
				{
					cmd_line_clear_input(cmd_line);
					char* reply = replies[5];
					for (int i = 0; i < strlen(reply); i++)
					{
						cmd_line_insert(cmd_line, reply[i]);
					}
				}
				keys->d = false;
			}
			else if (!keys->d)
			{
				keys->d = true;
			}
			break;

		case SDLK_g:
			if (keys->shift)
				editor_jump_bottom(editor);
			else if (keys->g)
			{
				editor_jump_top(editor);
				keys->g = false;
			}
			else if (!keys->g)
			{
				keys->g = true;
			}
			break;
	}
}

int main(int argc, char** argv)
{
	sdl_check(SDL_Init(SDL_INIT_VIDEO));
	sdl_check(TTF_Init());

	atexit(report_mem_leak);

	// Creating window
	Window* window = window_new("Text Editor", 800, 600);

	// List of buffers
	int curr_buffer = -1;
	int buffer_amt = 0;
	Editor** buffers = calloc(buffer_amt * buffer_amt, sizeof(Editor));
	
	// Generating settings
	Colors* colors_rgb = malloc(sizeof(Colors)); 
	Settings* settings = init_settings(colors_rgb, window,  buffers, &buffer_amt, &curr_buffer);

	// Loading font
	TTF_Font* font_1 = sdl_check_ptr(TTF_OpenFont(settings->family1, settings->font_size_1));
	TTF_Font* font_2 = sdl_check_ptr(TTF_OpenFont(settings->family2, settings->font_size_2));

	// Reading the cmd line arguments
	char file_name[256];
	for (int i = 0; i < 256; i++) file_name[i] = '\0';
	strcpy(file_name, "");

	if (argc > 1)
	{
		strcpy(file_name, argv[1]);
		add_new_buffer(window, settings, buffers, &buffer_amt, &curr_buffer, file_name, true);
	}
	
	// Keys for keybindings
	Keys* keys = init_keys();

	// Creating cmdline
	Cmd_line* cmd_line = cmd_line_new(window, font_2, settings);

	// Flags
	bool loop = true;
	bool halt = false;
	SDL_Event event;

	if (buffer_amt > 0)
		halt = true;

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
							if (settings->auto_indent) 
								editor_auto_indent(buffers[curr_buffer]);
							break;
						case SDLK_TAB:
							editor_insert(buffers[curr_buffer], '\t');
							break;
						case SDLK_BACKSPACE:
							editor_backspace(buffers[curr_buffer]);
							break;
						case SDLK_DELETE:
							editor_delete(buffers[curr_buffer]);
							break;

						// Mode switcher
						case SDLK_ESCAPE:
							window->mode = NORMAL;
							break;
					}
					arrow_movement(buffers[curr_buffer], event, keys, &curr_buffer, buffer_amt);
				}
				// NORMAL MODE SPECIFIC
				else if (window->mode == NORMAL)
				{
					switch (event.key.keysym.sym)
					{
						// Delete 
						case SDLK_DELETE:
						case SDLK_x:
							if (buffers[curr_buffer]->modifiable)
								editor_delete(buffers[curr_buffer]);
							else
							{
								cmd_line_clear_input(cmd_line);
								char* reply = replies[5];
								for (int i = 0; i < strlen(reply); i++)
								{
									cmd_line_insert(cmd_line, reply[i]);
								}
							}
							break;

						// Delete and change mode to insert
						case SDLK_s:
							if (buffers[curr_buffer]->modifiable)
							{
								editor_delete(buffers[curr_buffer]);
								window->mode = INSERT;
								halt = true;
							}
							else
							{
								cmd_line_clear_input(cmd_line);
								char* reply = replies[5];
								for (int i = 0; i < strlen(reply); i++)
								{
									cmd_line_insert(cmd_line, reply[i]);
								}
							}
							break;

						// Add new line above or below a line
						case SDLK_o:
							if (buffers[curr_buffer]->modifiable)
							{
								if (keys->shift)
									editor_insert_nl_abv(buffers[curr_buffer]);
								else
									editor_insert_nl_bel(buffers[curr_buffer]);
								if (settings->auto_indent) 
									editor_auto_indent(buffers[curr_buffer]);
								window->mode = INSERT;
								halt = true;
							}
							else
							{
								cmd_line_clear_input(cmd_line);
								char* reply = replies[5];
								for (int i = 0; i < strlen(reply); i++)
								{
									cmd_line_insert(cmd_line, reply[i]);
								}
							}
							break;

						// To jump to last character
						case SDLK_a:
							if (buffers[curr_buffer]->modifiable)
							{
								if (keys->shift)
								{
									editor_set_cur_back(buffers[curr_buffer]);
									window->mode = INSERT;
									halt = true;
								}
							}
							else
							{
								cmd_line_clear_input(cmd_line);
								char* reply = replies[5];
								for (int i = 0; i < strlen(reply); i++)
								{
									cmd_line_insert(cmd_line, reply[i]);
								}
							}
							break;
						
						// To paste
						case SDLK_p:
							if (buffers[curr_buffer]->modifiable)
							{
								editor_paste(buffers[curr_buffer]);
							}
							else
							{
								cmd_line_clear_input(cmd_line);
								char* reply = replies[5];
								for (int i = 0; i < strlen(reply); i++)
								{
									cmd_line_insert(cmd_line, reply[i]);
								}
							}
							break;

						// To create new buffer
						case SDLK_n:
							if (keys->ctrl)
							{
								add_new_buffer(window, settings, buffers, &buffer_amt, &curr_buffer, "", true);

								// Writing the msg in cmdline
								cmd_line_clear_input(cmd_line);
								char* reply = replies[6];
								for (int i = 0; i < strlen(reply); i++)
								{
									cmd_line_insert(cmd_line, reply[i]);
								}
							}
							break;

						// Mode switcher
						case SDLK_i:
							if (buffers[curr_buffer]->modifiable)
							{
								if (keys->shift)
									editor_set_cur_front(buffers[curr_buffer]);
								window->mode = INSERT;
								halt = true;
							}
							else
							{
								cmd_line_clear_input(cmd_line);
								char* reply = replies[5];
								for (int i = 0; i < strlen(reply); i++)
								{
									cmd_line_insert(cmd_line, reply[i]);
								}
							}
							break;

						case SDLK_v:
							if (keys->shift)
								editor_init_line_select(buffers[curr_buffer]);
							else
								editor_init_norm_select(buffers[curr_buffer]);

							window->mode = VISUAL;
							break;
					
						case SDLK_SEMICOLON:
							if (keys->shift)
							{
								window->mode = COMMAND;
								cmd_line_set_prompt(cmd_line, ":");
								cmd_line_clear_input(cmd_line);
								halt = true;
							}
							break;
					}
					hjkl_movement(buffers[curr_buffer], event, keys, &curr_buffer, buffer_amt, cmd_line);
					arrow_movement(buffers[curr_buffer], event, keys, &curr_buffer, buffer_amt);
				}
				// VISUAL MODE SPECIFIC
				else if (window->mode == VISUAL)
				{
					switch (event.key.keysym.sym)
					{
						// For copying
						case SDLK_y:
							editor_copy(buffers[curr_buffer]);
							window->mode = NORMAL;
							break;

						// For delete
						case SDLK_DELETE:
						case SDLK_x:
							editor_copy(buffers[curr_buffer]);
							
							if (buffers[curr_buffer]->modifiable)
							{
								editor_delete_sel(buffers[curr_buffer]);
							}
							else
							{
								cmd_line_clear_input(cmd_line);
								char* reply = replies[5];
								for (int i = 0; i < strlen(reply); i++)
								{
									cmd_line_insert(cmd_line, reply[i]);
								}
							}
							window->mode = NORMAL;
							break;

						// Replacing
						case SDLK_p:
							if (buffers[curr_buffer]->modifiable)
							{
								editor_replace_sel(buffers[curr_buffer]);
							}
							else
							{
								cmd_line_clear_input(cmd_line);
								char* reply = replies[5];
								for (int i = 0; i < strlen(reply); i++)
								{
									cmd_line_insert(cmd_line, reply[i]);
								}
							}
							window->mode = NORMAL;


						// Mode switching
						case SDLK_ESCAPE:
							window->mode = NORMAL;
							break;
					}
					hjkl_movement(buffers[curr_buffer], event, keys, &curr_buffer, buffer_amt, cmd_line);
					arrow_movement(buffers[curr_buffer], event, keys, &curr_buffer, buffer_amt);
				}
				// COMMAND MODE SPECIFIC
				else if (window->mode == COMMAND)
				{
					switch (event.key.keysym.sym)
					{
						case SDLK_RETURN:
							cmd_line_parse(cmd_line, buffers, &curr_buffer, &buffer_amt);
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

				// For key bindings
				switch (event.key.keysym.sym)
				{
					case SDLK_LCTRL:
					case SDLK_RCTRL:
						keys->ctrl = true;
						break;

					case SDLK_LSHIFT:
					case SDLK_RSHIFT:
						keys->shift = true;
						break;

					// Change the font size
					case SDLK_EQUALS:
						if (keys->ctrl)
						{
							settings->font_size_1++;
							editor_change_font(buffers[curr_buffer], 0, settings->family1, settings->font_size_1);
						}
						break;

					case SDLK_MINUS:
						if (keys->ctrl)
						{
							settings->font_size_1--;
							editor_change_font(buffers[curr_buffer], 0, settings->family1, settings->font_size_1);
						}
						break;
				}
			}
			else if (event.type == SDL_KEYUP)
			{
				// For key bindings
				switch (event.key.keysym.sym)
				{
					case SDLK_LCTRL:
					case SDLK_RCTRL:
						keys->ctrl = false;
						break;

					case SDLK_LSHIFT:
					case SDLK_RSHIFT:
						keys->shift = false;
						break;
				}
			}
			else if (event.type == SDL_WINDOWEVENT)
			{
				switch (event.window.event)
				{
					case SDL_WINDOWEVENT_RESIZED:
						SDL_GetWindowSize(window->window, &window->width, &window->height);
						cmd_line_resize(cmd_line);

						if (!halt)
							editor_resize(buffers[curr_buffer]);
						else
						{
							for (int i = 0 ; i < buffer_amt; i++)
								editor_resize(buffers[i]);
							halt = false;
						}
						break;
				}
			}

			if (buffer_amt <= 0)
			{
				loop = false;
			}
			else
			{
				window_clear(window, colors_rgb->editor_bg); 
				editor_render(buffers[curr_buffer], colors_rgb);
				cmd_line_render(cmd_line, font_2, colors_rgb);
			}
		}
	}

	// Freeing up the memory
	free(keys);
	free(buffers);
	free(colors_rgb);
	free_settings(settings);

	cmd_line_destroy(cmd_line);
	window_destroy(window);

	TTF_Quit();
	SDL_Quit();
	return 0;
}
