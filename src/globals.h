#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "includes.h"
#include "util.h"

// Keywords in C
static int c_data_types_len = 12;
static char* c_data_types[] = {
	"void", "int", "float", "char", "long",
	"short", "double", "signed", "unsigned", "size_t", 
	"const", "bool"
};

static int c_keywords_len = 16;
static char* c_keywords[] = {
	"auto",
	"break",
	"case",
	"continue",
	"default",
	"do",
	"else",
	"for",
	"goto",
	"if",
	"register",
	"return",
	"static",
	"switch",
	"volatile",
	"while"
};

static int c_functions_len = 25;
static char* c_functions[] = {
	"sizeof",
	"typedef",
	"union",
	"struct",
	"enum",
	"extern",
	"main",
	"printf",
	"fprintf",
	"sprintf",
	"scanf",
	"fscanf",
	"sscanf",
	"fopen",
	"popen",
	"fclose",
	"pclose",
	"strlen",
	"strcmp",
	"strcpy",
	"strcat",
	"strstr",
	"strchr",
	"memmove",
	"memcpy"
};

static char* c_bools[] = { "false", "true" };

// Keywords in python
static int py_data_types_len = 13;
static char* py_data_types[] = {
	"str", "int", "float", "complex",
	"list", "tuple", "dict",
	"set", "frozenset", "bool",
	"bytes", "bytearray", "memoryview"
};

static int py_keywords_len = 31;
static char* py_keywords[] = {
	"and",
	"as",
	"assert",
	"break",
	"class",	
	"continue",
	"def",
	"del",
	"elif",	
	"else",	
	"except",
	"finally",
	"for",
	"from",	
	"global",	
	"if",
	"import",	
	"in",
	"is",
	"lambda",	
	"None",
	"nonlocal",
	"not",
	"or",
	"pass",	
	"raise",	
	"return",
	"try",	
	"while",	
	"with",
	"yield"
};

static int py_functions_len = 12;
static char* py_functions[] = {
	"self",
	"print",
	"input",
	"len",
	"open",
	"write",
	"writelines",
	"read",
	"readlines",
	"append",
	"update",
	"range"
};

static char* py_bools[] = { "True", "False" };

static char special_char[] = " #!@$%^&*<>`~|\'\"(){}[]=+-,./\\:;%\n\t";

// Replies
static char replies[9][100] = {
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

	// Syntax
	SDL_Color comment;
	SDL_Color types;
	SDL_Color string;
	SDL_Color keywords;
	SDL_Color functions;
	SDL_Color symbols;
	SDL_Color constants;
} Colors;

// Settings
typedef struct
{
	// Fonts
	char family1[256];
	char family2[256];
	int font_size_1;
	int font_size_2;
	int def_font_size;

	bool auto_indent;
	bool syntax_on;
	int max_buff;
} Settings;

#endif
