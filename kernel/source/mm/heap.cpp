// heap.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace heap
{
	// design: list of sized buckets.
	// we need to have a manager managing the list of buckets,
	// then one that manages the list of memory chunks in each bucket.

	struct Bucket
	{
		// each bucket is responsible for exactly one chunk size.
		size_t chunkSize;

		// the bucket will have a list of chunks.
	};

	struct BucketList
	{

	};
}
}


namespace nx
{
	void* _allocator::allocate(size_t cnt, size_t align)
	{
		return nullptr;
	}

	void _allocator::deallocate(void* ptr)
	{

	}
}

























