// strdup.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.


#include "../../include/stdlib.h"
#include "../../include/string.h"

extern "C" char* strdup(const char* input)
{
	size_t inputsize = strlen(input);
	char* result = (char*) malloc(inputsize + 1);
	if(!result)
		return NULL;

	memcpy(result, input, inputsize + 1);
	return result;
}
