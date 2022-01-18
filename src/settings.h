#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "globals.h"
#include "editor.h"
#include "window.h"

// Adds the new buffer to the buffer list
static void add_new_buffer(Window* window, Settings* settings, Editor** buffers, int* buffer_amt, int* curr_buffer, char* file_name, bool modifiable)
{
	if (*buffer_amt + 1 < settings->max_buff)
	{
		++*buffer_amt;
		++*curr_buffer;

		Editor* new_buffer = editor_new(window, settings, file_name, modifiable);
		buffers[*curr_buffer] = new_buffer;

		// When new buffer is the **buffers**
		if (!strcmp(file_name, "**buffers**"))
		{
			editor_insert_str(buffers[*curr_buffer], "# These are the currently opened buffers. \n");
			for (int i = 0; i < *buffer_amt; i++)
			{
				char* file_name = buffers[i]->file_name;
				char index[3];
				sprintf(index, "%d: ", i);
				editor_insert_str(buffers[*curr_buffer], index);
				editor_insert_str(buffers[*curr_buffer], file_name);
				editor_insert(buffers[*curr_buffer], '\n');
			}
			buffers[*curr_buffer]->edited = false;
			buffers[*curr_buffer]->modifiable = false;
		}
	}
}

static Settings* init_settings(Colors* colors_rgb, Window* window, Editor** buffers, int* buffer_amt, int* curr_buffer)
{
	Settings* settings = malloc(sizeof(Settings));
	
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

	// Reading the config file
	struct passwd *passwdEnt = getpwuid(getuid());
	char *home = passwdEnt->pw_dir;
	char cf_path[] = "/.config/.editor/out/config\0";
	char full_path[strlen(home) + strlen(cf_path)];
	strcpy(full_path, home);
	strcat(full_path, cf_path);

	char temp[] = ".editor/out/config";

    FILE* fp = fopen(full_path, "r");
    if (fp == NULL)
	{
		fprintf(stderr, "Failed to read config file.\n");
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
			settings->def_font_size = atoi(value);
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
		else if (!strcmp(token, "SYNTAX_ON"))
		{
			char* value = strtok(NULL, "\n");
			if (!strcmp(value, "True"))
				settings->syntax_on = true;
			else if (!strcmp(value, "False"))
				settings->syntax_on = false;
			else
			{
				fprintf(stderr, "Incorrect value for syntax on: %s\n", value);
				exit(1);
			}
		}
		else if (!strcmp(token, "MAX_BUFF"))
		{
			char* value = strtok(NULL, "\n");
			if (is_no(value))
			{
				settings->max_buff = atoi(value);
			}
			else
			{
				fprintf(stderr, "Incorrect value for max buffers: %s\n", value);
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

		// Extracting syntax colors
		else if (!strcmp(token, "COMMENT"))
		{
			char* value = strtok(NULL, "\n");
			colors_rgb->comment = hex_to_rgb(value);
		}
		else if (!strcmp(token, "TYPES"))
		{
			char* value = strtok(NULL, "\n");
			colors_rgb->types = hex_to_rgb(value);
		}
		else if (!strcmp(token, "STRING"))
		{
			char* value = strtok(NULL, "\n");
			colors_rgb->string = hex_to_rgb(value);
		}
		else if (!strcmp(token, "KEYWORDS"))
		{
			char* value = strtok(NULL, "\n");
			colors_rgb->keywords = hex_to_rgb(value);
		}
		else if (!strcmp(token, "FUNCTIONS"))
		{
			char* value = strtok(NULL, "\n");
			colors_rgb->functions = hex_to_rgb(value);
		}
		else if (!strcmp(token, "SYMBOLS"))
		{
			char* value = strtok(NULL, "\n");
			colors_rgb->symbols = hex_to_rgb(value);
		}
		else if (!strcmp(token, "CONSTANTS"))
		{
			char* value = strtok(NULL, "\n");
			colors_rgb->constants = hex_to_rgb(value);
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

#endif
