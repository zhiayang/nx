// todo.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "../../include/stdio.h"


extern "C" int fscanf(FILE* str, const char* fmt, ...)
{
	(void) str;
	va_list args;
	va_start(args, fmt);
	va_end(args);

	return EOF;
}

extern "C" int scanf(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	va_end(args);

	return EOF;
}

extern "C" int sscanf(const char* s, const char* fmt, ...)
{
	(void) s;
	va_list args;
	va_start(args, fmt);
	va_end(args);

	return EOF;
}

extern "C" int remove(const char* path)
{
	(void) path;
	return 0;
}

extern "C" int rename(const char* oldname, const char* newname)
{
	(void) oldname;
	(void) newname;

	return -1;
}

extern "C" void setbuf(FILE*, char*)
{
}

extern "C" int setvbuf(FILE*, char*, int, size_t)
{
	return -1;
}

extern "C" FILE* tmpfile()
{
	return NULL;
}

extern "C" int ungetc(int c, FILE* str)
{
	(void) c;
	(void) str;

	return EOF;
}



















