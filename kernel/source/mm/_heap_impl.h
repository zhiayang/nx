// _heap_impl.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"


namespace nx {

template <size_t BucketCount, const size_t (&BucketSizes)[BucketCount], size_t InitialMultiplier,
	bool Expansion, bool LargeAllocations, bool Locked
>
struct heap_impl
{
	void set_name(const char* id) { HeapId = id; }
	const char* HeapId = 0;

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

	static constexpr size_t ExpansionFactor = 2;
	static constexpr size_t ChunksPerPage = PAGE_SIZE / sizeof(Chunk);


	// we always have a fixed number of buckets!
	Bucket Buckets[BucketCount];

	bool Initialised = false;

	size_t NumSpareChunks = 0;
	Chunk* SpareChunks = 0;

	size_t AllocatedByteCount = 0;


	/*
		why is this a spinlock, you ask, instead of a mutex? well, the reason is quite... convoluted. but simple,
		once you see why it's necessary. maybe it's not the best solution.

		imagine that thread A locks the heap (with a mutex!); in the middle of doing stuff while holding that lock,
		it gets pre-empted by the scheduler -- while holding that lock. this is normally fine.

		now, thread B comes along and wants to lock the heap also; except it can't, so it gets put to sleep with
		block(). what does block() do? it puts the thread into the blockedThreadList. adding things to a list can
		potentially (and, for the linked lists we use, will always) involve allocating memory. can you see now?

		thread B has called block(); we need to get more memory from the heap to append B to the list of blocked
		threads, so we try to lock the heap.

		except the heap is already locked, so we go to sleep by calling block()... you get the picture.

		a spinlock "solves" this problem by just... not calling block(). it just spins.
	*/
	nx::spinlock lock;



	addr_t internal_allocatePages(size_t num)
	{
		auto kproc = scheduler::getKernelProcess();
		assert(kproc);

		return vmm::allocateEager(num, vmm::AddressSpaceType::KernelHeap, 0, kproc);
	}


	void makeSpareChunks()
	{
		// make sure the lock is held.
		assert(Locked ? lock.held() : true);

		// make us one page worth of spare chunks -- 256 of them.
		addr_t chunkBuffer = internal_allocatePages(1);
		if(chunkBuffer == 0) abort("heap::makeSpareChunks(): out of memory!");

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

	Chunk* getSpareChunk()
	{
		// make sure the lock is held.
		assert(Locked ? lock.held() : true);

		if(SpareChunks == 0)
			makeSpareChunks();

		assert(NumSpareChunks > 0);

		auto ret = SpareChunks;
		SpareChunks = ret->next;

		return ret;
	}

	void expandBucket(Bucket* bucket)
	{
		// make sure the lock is held.
		assert(Locked ? lock.held() : true);

		if constexpr (!Expansion)
			return;

		addr_t memoryBuffer = internal_allocatePages(ExpansionFactor);
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
		// pretend to lock it for fun, because our helper functions assert that the lock is held (for safety)
		autolock lk(Locked ? &lock : 0);

		// clear everything first.
		memset(&Buckets[0], 0, sizeof(Bucket) * BucketCount);

		size_t numChunks[BucketCount];
		memset(&numChunks[0], 0, sizeof(size_t) * BucketCount);

		size_t numBytes = 0;

		// set up some calculations first.
		// basically, we allocate some bootstrap pages to store all the initial chunks.
		size_t slackSpace = 0;
		size_t numRequiredChunks = 0;
		for(size_t i = 0; i < BucketCount; i++)
		{
			size_t sz = BucketSizes[i];

			size_t n = ((InitialMultiplier * PAGE_SIZE) / sz);            // this truncates to an integer
			slackSpace += ((InitialMultiplier * PAGE_SIZE) - (n * sz));   // all the non-power-two sizes leave slack space

			numRequiredChunks += n;

			numChunks[i] = n;

			numBytes += sz * n;
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

		if(slackSpace != 0) abort("failed to consume slack space?! %zu bytes left", slackSpace);

		size_t numChunkPages = ((numRequiredChunks * sizeof(Chunk)) + PAGE_SIZE) / PAGE_SIZE;

		// each bucket gets exactly one page worth of memory for its chunks -- minus the slack space.
		// redistribution of wealth and all that
		addr_t chunkBuffer = internal_allocatePages(numChunkPages);
		addr_t memoryBuffer = internal_allocatePages(BucketCount);

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
		log(HeapId, "initialised with %zu chunks in %zu buckets (%zu bytes)", numRequiredChunks, BucketCount,
			numBytes);
	}

	bool initialised()
	{
		return Initialised;
	}


	void insertUsedChunk(Bucket* bucket, Chunk* chunk)
	{
		// make sure the lock is held.
		assert(Locked ? lock.held() : true);

		auto old = bucket->usedChunks;
		bucket->usedChunks = chunk;
		chunk->next = old;

		bucket->numUsedChunks += 1;
	}

	Bucket* getFittingBucket(size_t size)
	{
		// make sure the lock is held.
		assert(Locked ? lock.held() : true);

		// there has to be a more efficient way of doing this right??
		// like with bit shifting or smth??
		for(size_t i = 0; i < BucketCount; i++)
		{
			if(BucketSizes[i] >= size)
				return &Buckets[i];
		}

		return 0;
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

		align = __max(align, alignof(std::max_align_t));

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
		*/

		size_t extra_size = sizeof(size_t) + sizeof(alignment_offset_t) + (align - 1);
		size_t total_size = req_size + extra_size;

		addr_t return_ptr = 0;
		if(total_size >= PAGE_SIZE)
		{
			if constexpr (!LargeAllocations)
			{
				error(HeapId, "allocation size %zu too large!", total_size);
				return 0;
			}

			// note: no need to lock here, since the underlying page-allocator will
			// lock for us. we're not even using any of our chunks anyway.


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
			// lock. we can't put this in an ifconstexpr, if not RAII will not work.
			autolock lk(Locked ? &lock : 0);

			auto bucket = getFittingBucket(total_size);
			if(!bucket) abort("allocation of size %zu too large!", total_size);

			assert(bucket->chunkSize >= total_size);

			// see if we have chunks.
			if(bucket->numFreeChunks == 0)
				expandBucket(bucket);

			if(bucket->numFreeChunks == 0)
			{
				error(HeapId, "!! out of memory !!");
				util::printStackTrace();
				return 0;
			}

			// ok, we should have some now!
			assert(bucket->numFreeChunks > 0);

			// get the first one
			auto chunk = bucket->chunks;
			assert(chunk);
			assert(chunk->memory);

			bucket->chunks = chunk->next;
			bucket->numFreeChunks -= 1;

			addr_t ptr = chunk->memory;
			chunk->memory = 0;
			chunk->next = 0;

			insertUsedChunk(bucket, chunk);
			return_ptr = ptr;
		}

		// we don't need locks here, since we already obtained the memory.

		*((size_t*) return_ptr) = total_size;
		return_ptr += sizeof(size_t);

		auto ret = align_the_memory(return_ptr, align);

		AllocatedByteCount += total_size;
		return ret;
	}


	void deallocate(addr_t _addr)
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
			if constexpr (!LargeAllocations)
			{
				assert(false && "how tf did you manage this?!");
			}

			// again, we don't need to lock stuff here.


			// the size is still the size -- but we do some rounding to get the number of pages.
			assert(vmm::isAligned(addr));
			vmm::deallocate(addr, (sz + PAGE_SIZE) / PAGE_SIZE);
		}
		else
		{
			autolock lk(Locked ? &lock : 0);

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

			memset((void*) addr, 0, sz);


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

		AllocatedByteCount -= sz;
	}


	size_t getNumAllocatedBytes()
	{
		return AllocatedByteCount;
	}
};
}



/*
	design of this heap:

	* broad overview *

		We have a fixed number of Buckets (at the time of this writing, 16), each of which is responsible for Chunks of a
		particular size. The list of sizes that we support are outlined in the BucketSizes array. We start from 24 but generally
		increase in powers-of-two, while including the 1.5x values (eg. 24, 48, 96, etc)

		Unlike certain heaps where blocks get split/merged, or where there are headers/footers before/after blocks of memory, the
		memory in which Buckets and Chunks are managed are disjoint from the memory that we hand out to users.

		Each bucket manages a linked list of Chunks. each Chunk points points to a piece of memory, which is what we hand out to callers.


	* implementation *

		When allocating, we find the Bucket with the size that's a closest fit to the requested size -- plus some extra for bookkeeping.
		If the Bucket has run out of Chunks, we call expandBucket() to -- you guessed it -- expand the bucket. After that, we take the
		first Chunk (the head of the list), and give that out to the user.

		Of note is that the alignment offset is stored immediately before the user-given pointer (as a uint8_t), and the size of the chunk
		further before that, to let us find the correct Bucket when deallocating memory.

		We also keep a list of 'UsedChunks' in the Bucket, to which we add the chunk that was just allocated; while the contents of the chunk
		itself are now meaningless, we can reuse the chunk when deallocating memory.



		When deallocating, we do some pointer math to get both the alignment offset, as well as the total size of the chunk, so that we
		can find the Bucket that it belongs to. We then get a spare chunk from the UsedChunkList and make it point to the memory that
		we just received, before adding it back to the FreeChunkList.

		The invariant here is that, because chunks always move from one list to the other, and never out, we should never run out of
		existing chunks when deallocating memory.



		When expanding a bucket, we always expand by a fixed size -- currently 2 pages (or 8kb of memory). The variable in this case is
		the number of chunks required to address that memory. Since we are expanding a specific Bucket, we calculate the number of chunks
		that would be required (eg. in expanding a 2048-byte Bucket, we only need to create 4 chunks -- but we need 128 for a 64-byte Bucket).

		For the non-power-two sizes that we support, we will inevitably be left with some 'slack space', which we address by giving chunks to
		other bucket sizes. (we prioritise smaller chunks when redistributing this wealth, because they are more common)

		In order to refer to the memory that we have just sliced up, we use chunks from the 'SpareChunks' list -- which is a linked list (again)
		of -- you guessed it -- spare chunks. If the number of chunks that we need exceeds the number of spare chunks that we have, we make
		more spare chunks. Each time we make spare chunks we make 256 of them, because that's how many (ChunksPerPage) that fit in one page.

		We grab some chunks from the spare list, fix them to point to our memory, and add them to the list in their respective Buckets.
*/














