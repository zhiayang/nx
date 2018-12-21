// fgets.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "../../include/stdio.h"
#include "../../include/unistd.h"

#define FLAG_EOF	0x80
#define FLAG_ERR	0x40

extern "C" char* fgets(char* str, int num, FILE* stream)
{
	size_t read = fread(str, 1, num, stream);
	if(read < (size_t) num)
		stream->indicatorflags |= FLAG_EOF;

	if(read == 0)
		return NULL;

	return str;
}
