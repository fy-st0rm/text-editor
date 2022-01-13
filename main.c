#include "globals.h"
#include "editor.h"
#include "window.h"

/*
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
	char* file_name = "editor.c";
	Editor* editor = editor_new(window, file_name);

	editor_gen_texture(editor, window->renderer, font);
	editor_read_file(editor);

	bool loop = true;
	SDL_Event event;

	// Frame stuff
	int fps = 75;
	int frame_delay = 1000 / fps;
	Uint32 frame_start;
	int frame_time;

	while (loop)
	{
		if (SDL_WaitEvent(&event))
		{
			frame_start = SDL_GetTicks();

			if (event.type == SDL_QUIT)
				loop = false;
			else if (event.type == SDL_TEXTINPUT && window->mode == INSERT)
			{
				char* text = event.text.text;
				editor_insert(editor, text[0]);
			}
			else if (event.type == SDL_KEYDOWN)
			{
				// INSERT MODE SPECIFIC
				if (window->mode == INSERT)
				{
					switch (event.key.keysym.sym)
					{
						case SDLK_RETURN:
							editor_insert(editor, '\n');
							break;
						case SDLK_TAB:
							editor_insert(editor, '\t');
							break;
						case SDLK_BACKSPACE:
							editor_backspace(editor);
							break;

						// Mode switcher
						case SDLK_ESCAPE:
							window->mode = NORMAL;
							break;
					}
					arrow_movement(editor, event);
				}
				// NORMAL MODE SPECIFIC
				else if (window->mode == NORMAL)
				{
					switch (event.key.keysym.sym)
					{
						// Mode switcher
						case SDLK_i:
							window->mode = INSERT;
							break;
					}
					hjkl_movement(editor, event);
					arrow_movement(editor, event);
				}
			}
			else if (event.type == SDL_WINDOWEVENT)
			{
				switch (event.window.event)
				{
					case SDL_WINDOWEVENT_RESIZED:
						SDL_GetWindowSize(window->window, &window->width, &window->height);
						editor_resize(editor);
						break;
				}
			}

			window_clear(window, colors_rgb->editor_bg); 
			editor_render(editor, window, font, colors_rgb);

			// capping the frame rate
			frame_time = SDL_GetTicks() - frame_start;
			if (frame_delay > frame_time)
			{
				SDL_Delay(frame_delay - frame_time);		
			}
		}
	}

	free_colors();
	editor_destroy(editor);
	window_destroy(window);

	return 0;
}
