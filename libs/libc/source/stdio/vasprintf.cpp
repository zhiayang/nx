// vasprintf.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <stddef.h>
#include <stdarg.h>
#include "../../include/stdlib.h"
#include "../../include/string.h"
#include "../../include/defs/_printf.h"

struct vasprintf_state
{
	char* string;
	size_t string_length;
	size_t string_used;
};

static size_t vasprintf_callback(void* user, const char* string, size_t length)
{
	struct vasprintf_state* state = (vasprintf_state*) user;
	if(!state->string)
		return 0;

	size_t needed_length = state->string_used + length + 1;
	if(state->string_length < needed_length)
	{
		size_t new_length = 2 * state->string_used;
		if(new_length < needed_length)
			new_length = needed_length;
		size_t new_size = new_length * sizeof(char);
		char* new_string = (char*) realloc(state->string, new_size);

		if(!new_string)
		{
			free(state->string);
			state->string = NULL;
			return 0;
		}
		state->string = new_string;
		state->string_length = new_length;
	}
	memcpy(state->string + state->string_used, string, sizeof(char) * length);
	state->string_used += length;
	return length;
}

extern "C" int vasprintf(char** result_ptr, const char* format, va_list list)
{
	const size_t DEFAULT_SIZE = 32;
	struct vasprintf_state state;
	state.string_length = DEFAULT_SIZE;
	state.string_used = 0;
	if(!(state.string = (char*) malloc(state.string_length * sizeof(char))))
		return *result_ptr = NULL, -1;

	vcbprintf(&state, vasprintf_callback, format, list);
	if(!state.string)
		return *result_ptr = NULL, -1;

	state.string[state.string_used] = '\0';
	return *result_ptr = state.string, (int) state.string_used;
}
