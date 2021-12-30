#include "util.h"
#include "editor.h"
#include "window.h"

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
	SDL_Color bg = {165, 165, 165, 255};
	SDL_Color fg = {255, 0, 0, 255};

	while (loop)
	{
		if (SDL_WaitEvent(&event))
		{
			if 		(event.type == SDL_QUIT)	 loop = false;
			else if (event.type == SDL_TEXTINPUT)
			{
				char* text = event.text.text;
				editor_insert(editor, text[0]);
			}

			window_clear(window, bg); 
			editor_render_text(editor, window, fg, bg);
		}
	}

	editor_destroy(editor);
	window_destroy(window);

	return 0;
}