#include "util.h"
#include "editor.h"
#include "window.h"

/*
 * TODO: [ ] Fix the crash when moving cursor down when reached last line where last line is empty
 * TODO: [ ] Render only visible characters
 * TODO: [ ] Line buffer and command buffer
 * TODO: [ ] Read and write to the file
 * TODO: [ ] Clipboard handling
 * TODO: [X] Text insert with cursor position 
 * TODO: [X] Text deletion 
 * TODO: [X] Scrolling
 * TODO: [X] Cursor movement
 * TODO: [X] Fix a bug while inserting between the line
 */

int main()
{
	sdl_check(SDL_Init(SDL_INIT_VIDEO));
	sdl_check(TTF_Init());

	// Loading font
	char* font_path = "JetBrainsMonoNL-Regular.ttf";
	int font_size = 32;
	TTF_Font* font = sdl_check_ptr(TTF_OpenFont(font_path, font_size));

	Window* window = window_new("Text Editor", 800, 600);
	Editor* editor = editor_new(window);

	editor_gen_tex_cache(editor, window->renderer, font);
	for (int i = 0; i < MAX_TEXTURE; i++)
	{
		printf("%d %p\n", i, editor->texture_cache[i]);
	}

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
			/*
			else if (event.type == SDL_MOUSEWHEEL)
			{
				if (event.wheel.y > 0)
					editor_scroll_up(editor);
				if (event.wheel.y < 0)
					editor_scroll_down(editor);
			}
			*/
			else if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
					case SDLK_RETURN:
						editor_insert(editor, '\n');
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
						window->width  = event.window.data1;
						window->height = event.window.data2;
						editor_resize(editor, window);
						break;
				}
			}

			window_clear(window, bg); 
			editor_render_text(editor, window, font, fg, bg);
		}
	}

	editor_destroy(editor);
	window_destroy(window);

	return 0;
}
