// malloc.cpp
// Copyright (c) 2014 - 2016, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stddef.h>
#include "../../include/stdint.h"
#include "../../include/string.h"


namespace heap
{
	uintptr_t allocate(size_t req_size, size_t align);
	uintptr_t reallocate(uintptr_t old, size_t req_size, size_t align);
	void deallocate(uintptr_t _addr);
}

extern "C" void* malloc(size_t size)
{
	return (void*) heap::allocate(size, 1);
}

extern "C" void free(void* ptr)
{
	heap::deallocate((uintptr_t) ptr);
}

extern "C" void* calloc(size_t num, size_t size)
{
	void* ret = (void*) heap::allocate(num * size, 1);
	memset(ret, 0, num * size);

	return ret;
}

extern "C" void* realloc(void* ptr, size_t newsize)
{
	return (void*) heap::reallocate((uintptr_t) ptr, newsize, 1);
}



