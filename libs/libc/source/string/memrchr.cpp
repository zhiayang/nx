// memrchr.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <stddef.h>
extern "C" void* memrchr(const void* ptr, int c, size_t n)
{
	const unsigned char* buf = (const unsigned char*) ptr;
	for(size_t i = n; i != 0; i--)
		if(buf[i-1] == c)
				return (void*) (buf + i-1);
	return NULL;
}
