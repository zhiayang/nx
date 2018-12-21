// memccpy.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <stddef.h>
extern "C" void* memccpy(void* dest_ptr, const void* src_ptr, int c, size_t n)
{
	unsigned char* dest = (unsigned char*) dest_ptr;
	const unsigned char* src = (const unsigned char*) src_ptr;
	for(size_t i = 0; i < n; i++)
	{
		if((dest[i] = src[i]) == (unsigned char) c)
			return dest + i + 1;
	}
	return NULL;
}
