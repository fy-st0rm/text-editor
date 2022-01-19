#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "includes.h"
#include "util.h"

// Keywords in C
static char* c_data_types[] = {
	"void", "int", "float", "char", "long",
	"short", "double", "signed", "unsigned", "size_t", 
	"const", "bool"
};
static int c_data_types_len = sizeof(c_data_types) / sizeof(c_data_types[0]);

static char* c_keywords[] = {
	"register",
	"static",
	"volatile",
	"sizeof",
	"typedef",
	"union",
	"struct",
	"enum",
	"extern"
};
static int c_keywords_len = sizeof(c_keywords) / sizeof(c_keywords[0]);

static char* c_op_statements[] = {
	"switch",
	"while",
	"auto",
	"return",
	"break",
	"case",
	"continue",
	"default",
	"do",
	"else",
	"for",
	"goto",
	"if",
};
static int c_op_statements_len = sizeof(c_op_statements) / sizeof(c_op_statements[0]);

static char* c_functions[] = {
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
static int c_functions_len = sizeof(c_functions) / sizeof(c_functions[0]);

static char* c_bools[] = { "false", "true" };

// Keywords in python
static char* py_data_types[] = {
	"str", "int", "float", "complex",
	"list", "tuple", "dict",
	"set", "frozenset", "bool",
	"bytes", "bytearray", "memoryview"
};
static int py_data_types_len = sizeof(py_data_types) / sizeof(py_data_types[0]);

static char* py_keywords[] = {
	"assert",
	"global",	
	"import",	
	"None",
	"nonlocal",
	"raise",	
	"self",
	"and",
	"as",
	"from",	
	"not",
	"or",
	"in",
	"is"
};
static int py_keywords_len = sizeof(py_keywords) / sizeof(py_keywords[0]);

static char* py_op_statements[] = {
	"class",	
	"def",
	"del",
	"except",
	"finally",
	"for",
	"continue",
	"elif",	
	"else",	
	"break",
	"return",
	"try",	
	"while",	
	"with",
	"yield",
	"pass",	
	"lambda",	
	"if"
};
static int py_op_statements_len = sizeof(py_op_statements) / sizeof(py_op_statements[0]);

static char* py_functions[] = {
	"__init__",
	"__name__",
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
static int py_functions_len = sizeof(py_functions) / sizeof(py_functions[0]);

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
	SDL_Color op_statements;
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
