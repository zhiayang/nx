// heap.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "_heap_impl.h"

namespace nx {
namespace heap
{
	constexpr size_t BucketSizes[] = {
		24, 32, 48, 64, 96, 128, 192, 256, 384, 512, 768, 1024, 1536, 2048, 3072, 4096
	};

	constexpr size_t BucketCount        = sizeof(BucketSizes) / sizeof(BucketSizes[0]);
	constexpr bool EnableExpansion      = true;
	constexpr bool EnableLargeAllocs    = true;
	constexpr bool Locked               = true;
	constexpr size_t InitialMultiplier  = 1;

	static heap_impl<BucketCount, BucketSizes, InitialMultiplier, EnableExpansion, EnableLargeAllocs, Locked> impl;

	void init()
	{
		impl = decltype(impl)();
		impl.set_name("heap");
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
	void* _allocator::allocate(size_t cnt, size_t align)
	{
		return (void*) heap::allocate(cnt, align);
	}

	void _allocator::deallocate(void* ptr)
	{
		heap::deallocate((addr_t) ptr);
	}
}



























