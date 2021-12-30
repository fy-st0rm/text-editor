#ifndef _UTIL_H_
#define _UTIL_H_

#include "includes.h"

static int sdl_check(int result)
{
	if (result < 0)
	{
		fprintf(stderr, "ERROR: %s\n", SDL_GetError());
		exit(1);
	}
	return result;
}

static void* sdl_check_ptr(void* result)
{
	if (!result)
	{
		fprintf(stderr, "ERROR: %s\n", SDL_GetError());
		exit(1);
	}
	return result;
}

#endif
