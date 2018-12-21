// fprintf.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "../../include/stdio.h"
#include "../../include/limits.h"
#include "../../include/defs/_printf.h"

static size_t FileWriteCallback(void* user, const char* string, size_t stringlen)
{
	return fwrite(string, 1, stringlen, (FILE*) user);
}

extern "C" int fprintf(FILE* stream, const char* format, ...)
{
	va_list args;
	va_start(args, format);

	int ret = vfprintf(stream, format, args);
	va_end(args);

	return ret;
}

extern "C" int vfprintf(FILE* file, const char* format, va_list ap)
{
	size_t result = vcbprintf(file, FileWriteCallback, format, ap);
	if(result == LONG_MAX)
		return -1;
	return (int) result;
}
