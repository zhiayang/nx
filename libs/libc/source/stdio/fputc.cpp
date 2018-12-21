// fputc.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "../../include/stdio.h"
#include "../../include/unistd.h"

extern "C" int fputc(int c, FILE* str)
{
	fwrite(&c, sizeof(char), 1, str);
	return c;
}

extern "C" int putc(int c, FILE* str)
{
	return fputc(c, str);
}

extern "C" int putchar(int c)
{
	return fputc(c, stdout);
}
