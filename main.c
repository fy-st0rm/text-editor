#include "util.h"
#include "editor.h"
#include "window.h"

/*
 * TODO: [ ] Proper cursor movement
 * TODO: [ ] Fix the memory leak while resizing the window
 * TODO: [ ] Read and write to the file
 * TODO: [ ] Clipboard handling
 * TODO: [ ] Text selection
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

int main(int argc, char** argv)
{
	sdl_check(SDL_Init(SDL_INIT_VIDEO));
	sdl_check(TTF_Init());

	atexit(report_mem_leak);

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
	SDL_Color bg = {0, 0, 0, 255};
	SDL_Color fg = {255, 255, 255, 255};

	while (loop)
	{
		if (SDL_WaitEvent(&event))
		{
			if (event.type == SDL_QUIT)
				loop = false;
			else if (event.type == SDL_TEXTINPUT)
			{
				char* text = event.text.text;
				editor_insert(editor, text[0]);
			}
			else if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
					case SDLK_RETURN:
						editor_insert(editor, '\n');
						break;
					case SDLK_TAB:
						//for (int i = 0; i < 4; i++) editor_insert(editor, ' ');
						editor_insert(editor, '\t');
						break;
					case SDLK_BACKSPACE:
						editor_backspace(editor);
						break;

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

			window_clear(window, bg); 
			editor_render(editor, window, font, fg, bg);
		}
	}

	editor_destroy(editor);
	window_destroy(window);

	return 0;
}
