// heap.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace heap
{
	struct Chunk
	{
		Chunk* next;

		addr_t memory;
		size_t length;
	};

	struct Bucket
	{
		Chunk* head;
		size_t numChunks;
	};

	static constexpr size_t BucketCount = 16;
	static constexpr size_t BucketSizes[BucketCount] = {
		16, 24, 32, 48, 64, 96, 128, 192, 256, 384, 512, 768, 1024, 1536, 2048, 3072
	};

	/*
		these are the calculations for how many chunks we need to bootstrap the heap.

		16:     4096/16     = 256
		24:     4096/24     = 170.66    slack: 4096-(170*24) = 16
		32:     4096/32     = 128
		48:     4096/48     = 85.33     slack: 4096-(85*48) = 16
		64:     4096/64     = 64
		96:     4096/96     = 42.66     slack: 4096-(42*96) = 64
		128:    4096/128    = 32
		192:    4096/192    = 21.33     slack: 4096-(21*192) = 64
		256:    4096/256    = 16
		384:    4096/384    = 10.66     slack: 4096-(10*384) = 256
		512:    4096/512    = 8
		768:    4096/768    = 5.33      slack: 4096-(5*768) = 256
		1024:   4096/1024   = 4
		1536:   4096/1536   = 2.66      slack: 4096-(2*1536) = 1024
		2048:   4096/2048   = 2
		3072:   4096/3072   = 1.33      slack: 4096-(1*3072) = 1024

		our bootstrap memory can be allocated contiguously, so we can consolidate all the slack space and make
		new chunks with it.

		in this case, the base amount of chunks we will need is 256+170+128+85+64+42+32+21+16+10+8+5+4+2+2+1 = 846
		we will have 1024+1024+256+256+64+64+16+16 = 2720 bytes of slack space, which we can distribute as such:
		2720 - 2048 = 672
		672 - 512 = 160
		160 - 128 = 32
		32 - 32 = 0

		so we make 4 extra chunks, getting the total number of chunks up to 850.
		the number of pages we will need for this is 850 * sizeof(Chunk) aka 24 / 0x1000
		(850*24 + 4096)/4096 = 5.98046875

		or 5 pages.

		since we have 16 sizes, we will allocate 16 pages for the heap memory itself, using us a total of 21 pages (or 21*4096 = 86016 -- 86kb)


		when we expand the heap, we will need to do this "slack-space-management" on the fly.
	*/



	// we always have a fixed number of buckets!
	static Bucket buckets[BucketCount];

	void init()
	{
		// clear everything first.
		memset(&buckets[0], 0, sizeof(Bucket) * BucketCount);

		size_t numChunks[BucketCount];
		memset(&numChunks[0], 0, sizeof(size_t) * BucketCount);

		// set up some calculations first.
		// basically, we allocate some bootstrap pages to store all the initial chunks.
		size_t slackSpace = 0;
		size_t numRequiredChunks = 0;
		for(size_t i = 0; i < BucketCount; i++)
		{
			size_t sz = BucketSizes[i];

			size_t n = (PAGE_SIZE / sz);  // this truncates to an int.
			slackSpace += (PAGE_SIZE - (n * sz));

			numRequiredChunks += n;

			numChunks[i] = n;
		}

		// cut the slack space down -- prefer big chunks.
		for(size_t i = BucketCount; i-- > 0;)
		{
			size_t sz = BucketSizes[i];
			if(sz <= slackSpace)
			{
				slackSpace -= sz;

				numChunks[i] += 1;
				numRequiredChunks += 1;
			}
		}

		if(slackSpace != 0) abort("failed to consume slack space?!");

		// round up to nearest page. should be 5!
		size_t numChunkPages = ((numRequiredChunks * sizeof(Chunk)) + PAGE_SIZE) / PAGE_SIZE;

		addr_t chunkBuffer = vmm::allocate(numChunkPages, vmm::AddressSpace::KernelHeap);
		addr_t memoryBuffer = vmm::allocate(BucketCount, vmm::AddressSpace::KernelHeap);

		// set up the origin chunks
		for(size_t i = 0; i < BucketCount; i++)
		{
			// this is the number of chunks for this size that we will make.
			size_t num = numChunks[i];

			// there is no obligation to start from the head of the list. so, to keep thing simple,
			// we start from the tail, so the last one is the head.

			Chunk* next = 0;
			for(size_t k = 0; k < num; k++)
			{
				auto chunk = (Chunk*) chunkBuffer;

				chunk->next = next;
				chunk->length = BucketSizes[i];
				chunk->memory = memoryBuffer;

				chunkBuffer += sizeof(Chunk);
				memoryBuffer += BucketSizes[i];

				next = chunk;
			}

			// next should now be the head.
			buckets[i].head = next;
			buckets[i].numChunks = num;
		}

		println("heap initialised with %zu chunks", numRequiredChunks);
	}


	addr_t allocate(size_t size, size_t align)
	{
		return 0;
	}


	void deallocate(addr_t addr)
	{
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






























