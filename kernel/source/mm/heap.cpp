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
	};

	struct Bucket
	{
		Chunk* chunks;
		size_t numFreeChunks;

		Chunk* usedChunks;
		size_t numUsedChunks;

		size_t bucketIdx;
		size_t chunkSize;
	};

	static addr_t align_up(addr_t ptr, size_t align)
	{
		return (((ptr) + ((align) - 1)) & ~((align) - 1));
	}

	static constexpr size_t BucketSizes[] = {
		24, 32, 48, 64, 96, 128, 192, 256, 384, 512, 768, 1024, 1536, 2048, 3072, 4096
	};

	static constexpr size_t BucketCount = sizeof(BucketSizes) / sizeof(BucketSizes[0]);
	static constexpr size_t ChunksPerPage = PAGE_SIZE / sizeof(Chunk);

	// we always have a fixed number of buckets!
	static Bucket Buckets[BucketCount];

	static size_t NumSpareChunks = 0;
	static Chunk* SpareChunks = 0;

	static constexpr size_t ExpansionFactor = 2;


	static void makeSpareChunks()
	{
		// make us one page worth of spare chunks -- 256 of them.
		addr_t chunkBuffer = vmm::allocate(1, vmm::AddressSpace::KernelHeap);
		if(chunkBuffer == 0) abort("heap::makeSpareChunks(): out of memory!");

		Chunk* next = 0;
		for(size_t k = 0; k < ChunksPerPage; k++)
		{
			auto chunk = (Chunk*) chunkBuffer;

			chunk->next = next;
			chunk->memory = 0;

			chunkBuffer += sizeof(Chunk);

			next = chunk;
		}

		// 'next' is the head of the list of these.
		if(SpareChunks == 0)
		{
			SpareChunks = next;
		}
		else
		{
			auto old = SpareChunks;
			SpareChunks = next;
			next->next = old;
		}

		NumSpareChunks += ChunksPerPage;
	}

	static Chunk* getSpareChunk()
	{
		if(SpareChunks == 0)
			makeSpareChunks();

		assert(NumSpareChunks > 0);

		auto ret = SpareChunks;
		SpareChunks = ret->next;

		return ret;
	}

	static void expandBucket(Bucket* bucket)
	{
		addr_t memoryBuffer = vmm::allocate(ExpansionFactor, vmm::AddressSpace::KernelHeap);
		if(memoryBuffer == 0) abort("heap::expandBucket(): out of memory!");

		size_t numBytes = ExpansionFactor * PAGE_SIZE;
		size_t numChunks = numBytes / bucket->chunkSize;

		numBytes -= (numChunks * bucket->chunkSize);

		size_t chunkCounter[BucketCount];
		memset(&chunkCounter[0], 0, sizeof(size_t) * BucketCount);

		chunkCounter[bucket->bucketIdx] = numChunks;

		// now we have some slack space, consume it by making new chunks of other sizes.
		//* note! we start at index 3, which is the 64-bucket -- we don't want to make extra
		//* buckets of larger sizes, since smaller sizes are usually more common.
		for(size_t i = 3; i -- > 0;)
		{
			size_t sz = BucketSizes[i];
			if(sz <= numBytes)
			{
				numBytes -= sz;
				numChunks += 1;

				chunkCounter[i] += 1;
			}
		}

		// we need 'numChunks' worth of chunks.
		if(numChunks > NumSpareChunks)
			makeSpareChunks();

		assert(NumSpareChunks >= numChunks);

		// ok we got enough chunks now.
		for(size_t i = BucketCount; i-- > 0;)
		{
			// this is the number of chunks for this size that we will make.
			size_t num = chunkCounter[i];

			Chunk* next = Buckets[i].chunks;
			for(size_t k = 0; k < num; k++)
			{
				auto chunk = getSpareChunk();

				chunk->next = next;
				chunk->memory = memoryBuffer;

				memoryBuffer += BucketSizes[i];

				next = chunk;
			}

			Buckets[i].chunks = next;
			Buckets[i].numFreeChunks += num;
		}
	}






	void init()
	{
		// clear everything first.
		memset(&Buckets[0], 0, sizeof(Bucket) * BucketCount);

		size_t numChunks[BucketCount];
		memset(&numChunks[0], 0, sizeof(size_t) * BucketCount);

		// set up some calculations first.
		// basically, we allocate some bootstrap pages to store all the initial chunks.
		size_t slackSpace = 0;
		size_t numRequiredChunks = 0;
		for(size_t i = 0; i < BucketCount; i++)
		{
			size_t sz = BucketSizes[i];

			size_t n = (PAGE_SIZE / sz);            // this truncates to an integer
			slackSpace += (PAGE_SIZE - (n * sz));   // all the non-power-two sizes leave slack space

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

		size_t numChunkPages = ((numRequiredChunks * sizeof(Chunk)) + PAGE_SIZE) / PAGE_SIZE;

		// each bucket gets exactly one page worth of memory for its chunks -- minus the slack space.
		// redistribution of wealth and all that
		addr_t chunkBuffer = vmm::allocate(numChunkPages, vmm::AddressSpace::KernelHeap);
		addr_t memoryBuffer = vmm::allocate(BucketCount, vmm::AddressSpace::KernelHeap);

		// set up the origin chunks
		for(size_t i = BucketCount; i-- > 0;)
		{
			// this is the number of chunks for this size that we will make.
			size_t num = numChunks[i];

			// since this is a singly-linked list, the first chunk we make will be the tail,
			// and the last one we make will be the head. it's easier this way
			Chunk* next = 0;
			for(size_t k = 0; k < num; k++)
			{
				auto chunk = (Chunk*) chunkBuffer;

				chunk->next = next;
				chunk->memory = memoryBuffer;

				chunkBuffer += sizeof(Chunk);
				memoryBuffer += BucketSizes[i];

				next = chunk;
			}

			// next should now be the head.
			Buckets[i].chunks = next;
			Buckets[i].numFreeChunks = num;
			Buckets[i].chunkSize = BucketSizes[i];

			Buckets[i].usedChunks = 0;
			Buckets[i].numUsedChunks = 0;

			Buckets[i].bucketIdx = i;
		}

		println("heap initialised with %zu chunks in %zu buckets", numRequiredChunks, BucketCount);
	}



	static void insertUsedChunk(Bucket* bucket, Chunk* chunk)
	{
		auto old = bucket->usedChunks;
		bucket->usedChunks = chunk;
		chunk->next = old;

		bucket->numUsedChunks += 1;
	}



	using alignment_offset_t = uint8_t;
	static addr_t align_the_memory(addr_t ptr, size_t align)
	{
		auto tmp = ptr;
		ptr = align_up(ptr + sizeof(alignment_offset_t), align);

		// store the offset we used to get the alignment
		*(((alignment_offset_t*) ptr) - 1) = (alignment_offset_t) (ptr - tmp);

		return ptr;
	}

	addr_t allocate(size_t req_size, size_t align)
	{
		if(req_size == 0) return 0;

		assert((align & (align - 1)) == 0);
		assert(align <= 256);

		/*
			how we handle alignment is this:
			we store a byte of data 'behind' the returned pointer.
			we also need to store 8 bytes (size_t) for the size of the chunk.

			it looks like this:

			<   8 bytes   >   <   N bytes   >   < 2 b >

			S S S S S S S S   A A A A A A A A   X X X X   M M M M M M M M M M M
			---   size  ---   -- alignment --   - ofs -   ^ -- returned ptr --

			so we need to take into account all of the bookkeeping, such that the returned
			pointer is aligned correctly. 'ofs' stores the size of the alignment + the size of
			the offset count itself, so when we subtract that many from the received pointer,
			we will get the end of 'size'. we then subtract 8 (or sizeof(size_t)) to get the
			actual pointer we return.

			very complicated!
		*/

		size_t extra_size = sizeof(size_t) + sizeof(alignment_offset_t) + (align - 1);
		size_t total_size = req_size + extra_size;


		if(req_size >= PAGE_SIZE)
		{
			// note: we do have a bucket for 4096-byte chunks, but that's mainly
			// for allocations between 3072 and 4096 (where it would be wasteful to
			// allocate a page for it

			// the front of the chunk will be the size -- of the bucket!
			// note that we might waste a fuckload of memory on alignment for size requests just
			// very slightly above a page size! whatever.

			auto ptr = vmm::allocate((total_size + PAGE_SIZE) / PAGE_SIZE, vmm::AddressSpace::KernelHeap);

			*((size_t*) ptr) = total_size;
			ptr += sizeof(size_t);

			return align_the_memory(ptr, align);
		}
		else
		{
			size_t bucketIdx = 0;
			for(size_t i = 0; i < BucketCount; i++)
			{
				if(BucketSizes[i] >= total_size)
				{
					bucketIdx = i;
					break;
				}
			}

			auto bucket = &Buckets[bucketIdx];
			assert(bucket->chunkSize >= total_size);

			// see if we have chunks.
			if(bucket->numFreeChunks == 0)
				expandBucket(bucket);

			// ok, we should have some now!
			assert(bucket->numFreeChunks > 0);

			// get the first one
			auto chunk = bucket->chunks;

			bucket->chunks = chunk->next;
			bucket->numFreeChunks -= 1;

			insertUsedChunk(bucket, chunk);

			addr_t ptr = chunk->memory;
			chunk->memory = 0;
			chunk->next = 0;

			// the front of the chunk will be the size -- of the bucket!
			*((size_t*) ptr) = bucket->chunkSize;
			ptr += sizeof(size_t);

			return align_the_memory(ptr, align);
		}
	}


	void deallocate(addr_t addr)
	{
		if(addr == 0) return;

		// grab the uint8_t offset right behind the addr
		auto ofs = *(((uint8_t*) addr) - 1);

		addr -= ofs;
		addr -= sizeof(size_t);

		size_t sz = *((size_t*) addr);

		if(sz >= PAGE_SIZE)
		{
			// the size is still the size -- but we do some rounding to get the number of pages.
			assert(addr == (addr & vmm::PAGE_NO_FLAGS));
			vmm::deallocate(addr, (sz + PAGE_SIZE) / PAGE_SIZE);
		}
		else
		{
			// ok now we have the real pointer.
			// there has to be a more efficient way of doing this right??
			// like with bit shifting or smth??

			size_t bucketIdx = -1;
			for(size_t i = 0; i < BucketCount; i++)
			{
				if(BucketSizes[i] == sz)
				{
					bucketIdx = i;
					break;
				}
			}

			assert(bucketIdx >= 0);

			auto bucket = &Buckets[bucketIdx];

			// we should not run out of chunk!!!! the theory is obviously that every free() comes with an alloc(), and every
			// alloc() puts a chunk into the UsedChunks list!
			assert(bucket->numUsedChunks > 0);

			auto chunk = bucket->usedChunks;
			bucket->usedChunks = chunk->next;

			// set up the chunk
			chunk->next = 0;
			chunk->memory = addr;

			// put the chunk back into the free list.
			{
				auto old = bucket->chunks;
				chunk->next = old;
				bucket->chunks = chunk;

				bucket->numFreeChunks += 1;
				bucket->numUsedChunks -= 1;
			}

			// ok, we're done.
		}
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
	the number of pages we will need for this is 850 * sizeof(Chunk) / 0x1000
	(850*24 + 4096)/4096 = 5.98046875

	or 5 pages.

	since we have 16 sizes, we will allocate 16 pages for the heap memory itself, using us a total of 21 pages (or 21*4096 = 86016 -- 86kb)


	when we expand the heap, we will need to do this "slack-space-management" on the fly.
*/





























