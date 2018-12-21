// sprintf.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "../../include/stdio.h"
#include "../../include/limits.h"
#include "../../include/string.h"
#include "../../include/defs/_printf.h"



typedef struct vsnprintf_struct
{
	char* str;
	size_t size;
	size_t produced;
	size_t written;

} vsnprintf_t;

static size_t StringPrintCallback(void* user, const char* string, size_t stringlen)
{
	vsnprintf_t* info = (vsnprintf_t*) user;
	if(info->produced < info->size)
	{
		size_t available = info->size - info->produced;
		size_t possible = (stringlen < available) ? stringlen : available;
		memcpy(info->str + info->produced, string, possible);
		info->written += possible;
	}
	info->produced += stringlen;
	return stringlen;
}

extern "C" int snprintf(char* str, size_t size, const char* format, ...)
{
	va_list list;
	va_start(list, format);
	int result = vsnprintf(str, size, format, list);

	va_end(list);
	return result;
}

extern "C" int vsnprintf(char* str, size_t size, const char* format, va_list list)
{
	vsnprintf_t info;
	info.str = str;
	info.size = size ? size-1 : 0;
	info.produced = 0;
	info.written = 0;
	vcbprintf(&info, StringPrintCallback, format, list);
	if(size)
		info.str[info.written] = '\0';

	return (int) info.produced;
}







