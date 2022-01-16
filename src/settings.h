#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "globals.h"
#include "editor.h"
#include "window.h"


// Adds the new buffer to the buffer list
static void add_new_buffer(Window* window, Settings* settings, Editor** buffers, int* buffer_amt, int* curr_buffer, char* file_name, bool modifiable)
{
	++*buffer_amt;
	++*curr_buffer;

	Editor** new_buffers = calloc(*buffer_amt * *buffer_amt, sizeof(Editor));
	memcpy(new_buffers, buffers, sizeof(Editor) * *buffer_amt * *buffer_amt);

	Editor* new_buffer = editor_new(window, settings, file_name, modifiable);
	new_buffers[*curr_buffer] = new_buffer;

	memcpy(buffers, new_buffers, *buffer_amt * *buffer_amt * sizeof(Editor));
	free(new_buffers);
}

static Settings* init_settings(Colors* colors_rgb, Window* window, Editor** buffers, int* buffer_amt, int* curr_buffer)
{
	Settings* settings = malloc(sizeof(Settings));
	
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

	// Reading the config file
    FILE* fp = fopen("config/out/config", "r");
    if (fp == NULL)
	{
		fprintf(stderr, "Failed to read config file.");
		exit(1);
	}

	// Reading line by line
    while ((read = getline(&line, &len, fp)) != -1) {
		char* token = strtok(line, " ");
	
		// Interpreting the lines
		if (!strcmp(token, "FAMILY1"))
		{
			char* value = strtok(NULL, "\n");
			strcpy(settings->family1, value);
		}
		else if (!strcmp(token, "FAMILY2"))
		{
			char* value = strtok(NULL, "\n");
			strcpy(settings->family2, value);
		}
		else if (!strcmp(token, "FONT_SIZE_1"))
		{
			char* value = strtok(NULL, "\n");
			settings->font_size_1 = atoi(value);
		}
		else if (!strcmp(token, "FONT_SIZE_2"))
		{
			char* value = strtok(NULL, "\n");
			settings->font_size_2 = atoi(value);
		}
		else if (!strcmp(token, "AUTO_INDENT"))
		{
			char* value = strtok(NULL, "\n");
			if (!strcmp(value, "True"))
				settings->auto_indent = true;
			else if (!strcmp(value, "False"))
				settings->auto_indent = false;
			else
			{
				fprintf(stderr, "Incorrect value for auto indent: %s\n", value);
				exit(1);
			}
		}
		// Extracting colors
		else if (!strcmp(token, "EDITOR_BG"))
		{
			char* value = strtok(NULL, "\n");
			colors_rgb->editor_bg = hex_to_rgb(value);
		}
		else if (!strcmp(token, "LINE_BG"))
		{
			char* value = strtok(NULL, "\n");
			colors_rgb->line_bg = hex_to_rgb(value);
		}
		else if (!strcmp(token, "BAR_BG"))
		{
			char* value = strtok(NULL, "\n");
			colors_rgb->bar_bg = hex_to_rgb(value);
		}
		else if (!strcmp(token, "EDITOR_FG"))
		{
			char* value = strtok(NULL, "\n");
			colors_rgb->editor_fg = hex_to_rgb(value);
		}
		else if (!strcmp(token, "LINE_FG"))
		{
			char* value = strtok(NULL, "\n");
			colors_rgb->line_fg = hex_to_rgb(value);
		}
		else if (!strcmp(token, "BAR_FG"))
		{
			char* value = strtok(NULL, "\n");
			colors_rgb->bar_fg = hex_to_rgb(value);
		}
		else if (!strcmp(token, "CUR_LINE_FG"))
		{
			char* value = strtok(NULL, "\n");
			colors_rgb->cur_line_fg = hex_to_rgb(value);
		}
		else if (!strcmp(token, "NORMAL_MD"))
		{
			char* value = strtok(NULL, "\n");
			colors_rgb->normal_md = hex_to_rgb(value);
		}
		else if (!strcmp(token, "INSERT_MD"))
		{
			char* value = strtok(NULL, "\n");
			colors_rgb->insert_md = hex_to_rgb(value);
		}
		else if (!strcmp(token, "VISUAL_MD"))
		{
			char* value = strtok(NULL, "\n");
			colors_rgb->visual_md = hex_to_rgb(value);
		}
		else if (!strcmp(token, "COMMAND_MD"))
		{
			char* value = strtok(NULL, "\n");
			colors_rgb->command_md = hex_to_rgb(value);
		}
		else if (!strcmp(token, "CURSOR"))
		{
			char* value = strtok(NULL, "\n");
			colors_rgb->cursor = hex_to_rgb(value);
		}
		else if (!strcmp(token, "SELECTION"))
		{
			char* value = strtok(NULL, "\n");
			colors_rgb->selection = hex_to_rgb(value);
		}

		// Interpreting about buffers
		else if (!strcmp(token, "BUFFER_NEW"))
		{
			char* file_name = strtok(NULL, " ");
			char* modifiable = strtok(NULL, "\n");

			bool mod = false;
			if (!strcmp(modifiable, "True"))
				mod = true;

			add_new_buffer(window, settings, buffers, buffer_amt, curr_buffer, file_name, mod);
		}
    }

    fclose(fp);
    if (line)
        free(line);

	return settings;
}

static void free_settings(Settings* settings)
{
	free(settings);
}

#endif
