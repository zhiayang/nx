// fixed_heap.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "_heap_impl.h"

namespace nx {
namespace fixed_heap
{
	constexpr size_t BucketSizes[] = {
		32, 64, 128, 256
	};

	constexpr size_t BucketCount        = sizeof(BucketSizes) / sizeof(BucketSizes[0]);
	constexpr bool EnableExpansion      = false;
	constexpr bool EnableLargeAllocs    = false;
	constexpr bool Locked               = false;
	constexpr size_t InitialMultiplier  = 2;

	static heap_impl<BucketCount, BucketSizes, InitialMultiplier, EnableExpansion, EnableLargeAllocs, Locked> impl;

	void init()
	{
		impl = decltype(impl)();
		impl.set_name("fixed_heap");
		impl.init();
	}

	addr_t allocate(size_t size, size_t align)
	{
		return impl.allocate(size, align);
	}

	void deallocate(addr_t addr)
	{
		return impl.deallocate(addr);
	}

	bool initialised()
	{
		return impl.initialised();
	}

	size_t getNumAllocatedBytes()
	{
		return impl.getNumAllocatedBytes();
	}
}
}


namespace nx
{
	void* _fixed_allocator::allocate(size_t cnt, size_t align)
	{
		return (void*) fixed_heap::allocate(cnt, align);
	}

	void _fixed_allocator::deallocate(void* ptr)
	{
		fixed_heap::deallocate((addr_t) ptr);
	}
}




























