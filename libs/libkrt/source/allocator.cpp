// allocator.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "krt.h"

namespace krt
{
	void* kernel_allocator::allocate(size_t sz)
	{
		return krt::heap::alloc(sz);
	}

	void kernel_allocator::deallocate(void* ptr)
	{
		krt::heap::free(ptr);
	}
}

