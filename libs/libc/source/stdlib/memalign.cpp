// memalign.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include <errno.h>
#include <stdint.h>
#include <stddef.h>

namespace heap
{
	uintptr_t allocate(size_t req_size, size_t align);
	uintptr_t reallocate(uintptr_t old, size_t req_size, size_t align);
	void deallocate(uintptr_t _addr);
}


extern "C" int posix_memalign(void** memptr, size_t alignment, size_t size)
{
	if(memptr == NULL || ((alignment % sizeof(void*)) != 0))
		return EINVAL;

	auto ptr = heap::allocate(size, alignment);

	if(ptr == 0)
		return ENOMEM;

	*memptr = (void*) ptr;
	return 0;
}

extern "C" void* aligned_alloc(size_t alignment, size_t size)
{
	void* ptr = 0;
	posix_memalign(&ptr, alignment, size);

	return ptr;
}

extern "C" void* memalign(size_t alignment, size_t size)
{
	void* ptr = 0;
	posix_memalign(&ptr, alignment, size);

	return ptr;
}
