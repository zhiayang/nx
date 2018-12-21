// printf.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "../../include/stdio.h"
#include "../../include/limits.h"


extern "C" int printf(const char* format, ...)
{
	va_list list;
	va_start(list, format);
	int result = vprintf(format, list);
	va_end(list);
	return result;
}

extern "C" int vprintf(const char* format, va_list list)
{
	return vfprintf(stdout, format, list);
}

extern "C" int sprintf(char* str, const char* format, ...)
{
	va_list list;
	va_start(list, format);
	int result = vsprintf(str, format, list);

	va_end(list);
	return result;
}

extern "C" int vsprintf(char* str, const char* format, va_list ap)
{
	return vsnprintf(str, LONG_MAX, format, ap);
}

extern "C" int asprintf(char** result_ptr, const char* format, ...)
{
	va_list list;
	va_start(list, format);
	int result = vasprintf(result_ptr, format, list);
	va_end(list);
	return result;
}

