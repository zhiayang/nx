// malloc.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <stddef.h>
#include "../../include/stdint.h"
#include "../../include/string.h"
#include "../../include/stdio.h"

#include "../../include/assert.h"

namespace Heap
{
	void* Allocate(size_t Size);
	void Free(void* Pointer);
	void* Reallocate(void* ptr, size_t newsize);
}

extern "C" void* malloc(size_t size)
{
	return Heap::Allocate(size);
}

extern "C" void free(void* ptr)
{
	Heap::Free(ptr);
}

extern "C" void* calloc(size_t num, size_t size)
{
	void* ret = Heap::Allocate(num * size);
	memset(ret, 0, num * size);

	return ret;
}

extern "C" void* realloc(void* ptr, size_t newsize)
{
	return Heap::Reallocate(ptr, newsize);
}



