// puts.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "../../include/stdio.h"
#include "../../include/string.h"

extern "C" int puts(const char* str)
{
	fwrite(str, 1, strlen(str), stdout);
	fputc('\n', stdout);

	return 1;
}

extern "C" int fputs(const char* s, FILE* stream)
{
	fwrite(s, sizeof(char), strlen(s), stream);
	return 0;
}
