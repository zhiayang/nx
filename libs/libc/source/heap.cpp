// heap.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

//* note: this is literally the same thing as what the kernel uses
//* just adapted to use mmap instead of vmm::*, plus support for realloc()
//* which we omit from the kernel because (a) it adds complexity, and (b) we don't need it.

// TODO: we need to handle out-of-memory conditions better!!
//? malloc & friends should return 0 on failure, not abort the program!

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <sys/mman.h>

namespace heap
{
	struct Chunk
	{
		Chunk* next;
		uintptr_t memory;
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

	static uintptr_t align_up(uintptr_t ptr, size_t align)
	{
		return (((ptr) + ((align) - 1)) & ~((align) - 1));
	}

	constexpr size_t BucketSizes[] = {
		24, 32, 48, 64, 96, 128, 192, 256, 384, 512, 768, 1024, 1536, 2048, 3072, 4096
	};

	using __max_align_t = long double;

	// cannot change!!!
	constexpr size_t PAGE_SIZE = 0x1000;
	constexpr uintptr_t PAGE_ALIGN = ~0xFFFULL;

	constexpr size_t ExpansionFactor = 2;
	constexpr size_t BucketCount = sizeof(BucketSizes) / sizeof(BucketSizes[0]);
	constexpr size_t ChunksPerPage = PAGE_SIZE / sizeof(Chunk);


	// we always have a fixed number of buckets!
	static Bucket Buckets[BucketCount];

	static bool Initialised = false;

	static size_t NumSpareChunks = 0;
	static Chunk* SpareChunks = 0;


	static uintptr_t NextVirtAddr = 0;
	static uintptr_t internal_allocatePages(size_t num)
	{
		void* at = (void*) NextVirtAddr;
		return (uintptr_t) mmap(at, num * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | (at ? MAP_FIXED : 0), 0, 0);
	}


	static void makeSpareChunks()
	{
		// make us one page worth of spare chunks -- 256 of them.
		uintptr_t chunkBuffer = internal_allocatePages(1);
		if(chunkBuffer == 0) assert(!"heap::makeSpareChunks(): out of memory!");

		// memset((void*) chunkBuffer, 0, 1 * PAGE_SIZE);

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
		uintptr_t memoryBuffer = internal_allocatePages(ExpansionFactor);
		if(memoryBuffer == 0) assert(!"heap::expandBucket(): out of memory!");


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

		if(slackSpace != 0) assert(!"failed to consume slack space?!");

		size_t numChunkPages = ((numRequiredChunks * sizeof(Chunk)) + PAGE_SIZE) / PAGE_SIZE;

		// each bucket gets exactly one page worth of memory for its chunks -- minus the slack space.
		// redistribution of wealth and all that
		uintptr_t chunkBuffer = internal_allocatePages(numChunkPages);
		uintptr_t memoryBuffer = internal_allocatePages(BucketCount);

		// memset((void*) chunkBuffer, 0, numChunkPages * PAGE_SIZE);

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

		Initialised = true;
	}

	bool initialised()
	{
		return Initialised;
	}


	static void insertUsedChunk(Bucket* bucket, Chunk* chunk)
	{
		auto old = bucket->usedChunks;
		bucket->usedChunks = chunk;
		chunk->next = old;

		bucket->numUsedChunks += 1;
	}

	static Bucket* getFittingBucket(size_t size)
	{
		for(size_t i = 0; i < BucketCount; i++)
		{
			if(BucketSizes[i] >= size)
				return &Buckets[i];
		}

		assert(!"did not find a fitting bucket?!");
		return 0;
	}



	using alignment_offset_t = uint8_t;
	static uintptr_t align_the_memory(uintptr_t ptr, size_t align)
	{
		auto tmp = ptr;
		ptr = align_up(ptr + sizeof(alignment_offset_t), align);

		// store the offset we used to get the alignment
		*(((alignment_offset_t*) ptr) - 1) = (alignment_offset_t) (ptr - tmp);

		return ptr;
	}


	uintptr_t allocate(size_t req_size, size_t align)
	{
		if(req_size == 0) return 0;

		align = (align, alignof(__max_align_t));

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

		uintptr_t return_ptr = 0;
		if(total_size >= PAGE_SIZE)
		{
			// note: we do have a bucket for 4096-byte chunks, but that's mainly
			// for allocations between 3072 and 4096 (where it would be wasteful to
			// allocate a page for it

			// the front of the chunk will be the size -- of the bucket!
			// note that we might waste a fuckload of memory on alignment for size requests just
			// very slightly above a page size! whatever.

			size_t num = (total_size + PAGE_SIZE) / PAGE_SIZE;
			return_ptr = internal_allocatePages(num);
		}
		else
		{
			auto bucket = getFittingBucket(total_size);
			assert(bucket->chunkSize >= total_size);

			// see if we have chunks.
			if(bucket->numFreeChunks == 0)
				expandBucket(bucket);

			// ok, we should have some now!
			assert(bucket->numFreeChunks > 0);

			// get the first one
			auto chunk = bucket->chunks;
			assert(chunk);
			assert(chunk->memory);

			bucket->chunks = chunk->next;
			bucket->numFreeChunks -= 1;

			uintptr_t ptr = chunk->memory;
			chunk->memory = 0;
			chunk->next = 0;

			insertUsedChunk(bucket, chunk);
			return_ptr = ptr;
		}

		*((size_t*) return_ptr) = total_size;
		return_ptr += sizeof(size_t);

		auto ret = align_the_memory(return_ptr, align);

		return ret;
	}


	void deallocate(uintptr_t _addr)
	{
		if(_addr == 0) return;

		auto addr = _addr;

		// grab the uint8_t offset right behind the addr
		auto ofs = *(((uint8_t*) addr) - 1);

		addr -= ofs;
		addr -= sizeof(size_t);

		size_t sz = *((size_t*) addr);
		assert(sz);

		if(sz >= PAGE_SIZE)
		{
			// the size is still the size -- but we do some rounding to get the number of pages.
			assert(addr == (addr & PAGE_ALIGN));
			munmap((void*) addr, sz);
		}
		else
		{
			// ok now we have the real pointer.
			// there has to be a more efficient way of doing this right??
			// like with bit shifting or smth??

			auto bucket = getFittingBucket(sz);

			// we should not run out of chunks!!!! the theory is obviously that every free() comes with an alloc(), and every
			// alloc() puts a chunk into the UsedChunks list!
			assert(bucket->numUsedChunks > 0);
			assert(bucket->usedChunks);

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


	uintptr_t reallocate(uintptr_t usr_addr, size_t req_size, size_t align)
	{
		if(usr_addr == 0)
			return allocate(req_size, align);

		auto base_addr = usr_addr;

		// grab the uint8_t offset right behind the addr
		auto ofs = *(((uint8_t*) base_addr) - 1);

		base_addr -= ofs;
		base_addr -= sizeof(size_t);

		size_t old_sz = *((size_t*) base_addr);
		size_t old_real_sz = old_sz - (usr_addr - base_addr);

		// we currently do not support shrinking.
		if(req_size <= old_sz)
			return usr_addr;

		if(old_sz >= PAGE_SIZE)
		{
			auto newptr = allocate(req_size, align);
			memcpy((void*) newptr, (void*) usr_addr, old_real_sz);

			deallocate(usr_addr);

			return newptr;
		}
		else
		{
			// figure out which bucket the current allocation belongs to
			auto bucket = getFittingBucket(old_sz);
			assert(bucket);

			// get the size of our bucket.
			auto bucket_sz = bucket->chunkSize;

			// get the total_size of the new allocation
			size_t extra_size = sizeof(size_t) + sizeof(alignment_offset_t) + (align - 1);
			size_t total_size = req_size + extra_size;

			if(bucket_sz >= total_size)
			{
				// oh look, we have enough space in the current bucket to accommodate everyone.
				// update the size & alignment, then return the same memory (potentially a different
				// address if the alignment was not the same as the original)

				uintptr_t return_ptr = base_addr;

				*((size_t*) return_ptr) = total_size;
				return_ptr += sizeof(size_t);

				auto ret = align_the_memory(return_ptr, align);

				return ret;
			}
			else
			{
				// boohoo, we need to make new memory.

				auto newptr = allocate(req_size, align);
				memcpy((void*) newptr, (void*) usr_addr, old_real_sz);

				deallocate(usr_addr);

				return newptr;
			}
		}
	}
}


























