// memmove.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <stddef.h>
extern "C" void* memmove(void* dest_ptr, const void* src_ptr, size_t n)
{
	unsigned char* dest = (unsigned char*) dest_ptr;
	const unsigned char* src = (const unsigned char*) src_ptr;
	if(dest < src)
		for(size_t i = 0; i < n; i++)
			dest[i] = src[i];
	else
		for(size_t i = n; i != 0; i--)
			dest[i - 1] = src[i - 1];

	return dest_ptr;
}
