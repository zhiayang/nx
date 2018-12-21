// memchr.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <stddef.h>

extern "C" void* memchr(const void* s, int c, size_t size)
{
	const unsigned char* buf = (const unsigned char*) s;
	for(size_t i = 0; i < size; i++)
		if(buf[i] == c)
			return (void*) (buf + i);
	return NULL;
}
