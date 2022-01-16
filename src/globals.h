#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "includes.h"
#include "util.h"

// Editor modes
enum Modes
{
	NORMAL = 'N',
	INSERT = 'I',
	VISUAL = 'V',
	COMMAND= 'C'
};

// Replies
static const char replies[9][100] = {
	{"S0: File saved sucessfully."},
	{"E1: No file name."},
	{"E2: Failed to open file."},
	{"S1: File read sucessfully."},
	{"E3: File is not saved. (add ! to exit without saving)"},
	{"E4: Buffer is not modifiable."},
	{"S2: Created new buffer."},
	{"S3: Sucessfully destroyed the buffer."},
	{"E5: Invalid command."}
};

// Settings
typedef struct
{
	// Fonts
	char family1[256];
	char family2[256];
	int font_size_1;
	int font_size_2;

	bool auto_indent;
} Settings;

// Colors
typedef struct 
{
	// Background
	SDL_Color editor_bg;
	SDL_Color line_bg;
	SDL_Color bar_bg;

	// Foreground
	SDL_Color editor_fg;
	SDL_Color line_fg;
	SDL_Color cur_line_fg;
	SDL_Color bar_fg;

	// Mode colors
	SDL_Color normal_md;
	SDL_Color insert_md;
	SDL_Color visual_md;
	SDL_Color command_md;

	// Cursor
	SDL_Color cursor;
	SDL_Color selection;
} Colors;


static Settings* init_settings(Colors* colors_rgb)
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
