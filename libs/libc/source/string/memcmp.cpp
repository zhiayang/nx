// memcmp.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <stddef.h>

extern "C" int memcmp(const void* a_ptr, const void* b_ptr, size_t size)
{
	const unsigned char* a = (const unsigned char*) a_ptr;
	const unsigned char* b = (const unsigned char*) b_ptr;
	for(size_t i = 0; i < size; i++)
	{
		if(a[i] < b[i])
			return -1;
		if(a[i] > b[i])
			return +1;
	}
	return 0;
}

