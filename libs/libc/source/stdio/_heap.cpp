// _heap.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "../../include/stdint.h"
#include "../../include/string.h"
#include "../../include/stdlib.h"
#include "../../include/assert.h"
#include "../../include/stdio.h"
#include "../../include/sys/mman.h"

namespace Heap
{
	// plugs:
	#define GetPage()		((uint64_t) mmap(0, 0x1000, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, 0, 0))
	#define GetPageFixed(x)	((uint64_t) mmap((void*) (x), 0x1000, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_FIXED | MAP_ANON, 0, 0))
	#define fail()			abort()

	// book keeping
	static uint64_t ChunksInHeap;
	static uint64_t LastFree;

	static uint64_t MetadataAddr;
	static uint64_t HeapAddress;

	static uint64_t SizeOfHeap;
	static uint64_t SizeOfMeta;

	const uint64_t MetaOffset = 64;
	const uint64_t Alignment = 64;

	struct Chunk
	{
		uint64_t offset;
		uint64_t size;
	};

	uint64_t addr(Chunk* c)
	{
		return (uint64_t) c;
	}

	Chunk* chunk(uint64_t addr)
	{
		return (Chunk*) addr;
	}

	Chunk* array()
	{
		return (Chunk*) (MetadataAddr + MetaOffset);
	}

	bool isfree(Chunk* c)
	{
		return c->size & 0x1;
	}

	void setfree(Chunk* c)
	{
		c->size |= 0x1;
	}

	void setused(Chunk* c)
	{
		c->size &= ~0x1;
	}

	uint64_t size(Chunk* c)
	{
		return c->size & ~0x1;
	}

	uint64_t midpoint(uint64_t a, uint64_t b)
	{
		return a + ((b - a) / 2);
	}

	static void CheckAndExpandMeta()
	{
		if((ChunksInHeap + 2) * sizeof(Chunk) > SizeOfMeta * 0x1000)
		{
			// we need to expand.
			uint64_t fixed = GetPageFixed(MetadataAddr + (SizeOfMeta * 0x1000));
			assert(fixed == MetadataAddr + (SizeOfMeta * 0x1000));
			SizeOfMeta++;
		}
	}


	Chunk* index(uint64_t i)
	{
		Chunk* arr = (Chunk*) (MetadataAddr + MetaOffset);
		if(i < ChunksInHeap)
			return &arr[i];

		else
			return nullptr;
	}

	void pushback(uint64_t at)
	{
		// memcpy them behind.
		// but first, check if we have enough space.
		CheckAndExpandMeta();
		auto behind = ChunksInHeap - at;

		// memcpy.
		memmove((void*) addr(index(at + 1)), (void*) index(at), behind * sizeof(Chunk));
		memset((void*) index(at), 0, sizeof(Chunk));
	}

	void pullfront(uint64_t at)
	{
		// essentially deletes a chunk.
		// 'at' contains the index of the chunk to delete.
		void* c = index(at);
		CheckAndExpandMeta();

		if(at == ChunksInHeap - 1)
			return;

		// calculate how many chunks to pull
		auto ahead = (ChunksInHeap - 1) - at;

		memset(c, 0, sizeof(Chunk));
		memmove(c, index(at + 1), ahead * sizeof(Chunk));
	}





	uint64_t round(uint64_t s)
	{
		uint64_t remainder = s % Alignment;

		if(remainder == 0)
			return s;

		return s + Alignment - remainder;
	}






	uint64_t bsearch(uint64_t key, uint64_t (*getkey)(uint64_t ind))
	{
		uint64_t imin = 0;
		uint64_t imax = ChunksInHeap - 1;
		uint64_t remaining = ChunksInHeap;

		while(imax >= imin && remaining > 0)
		{
			// calculate the midpoint for roughly equal partition
			auto imid = midpoint(imin, imax);

			// determine which subarray to search
			if(getkey(imid) < key)
			{
				if(imid < ChunksInHeap)
					imin = imid + 1;

				else
					break;
			}

			else if(getkey(imid) > key)
			{
				if(imid > 0)
					imax = imid - 1;

				else
					break;
			}

			else if(getkey(imid) == key)
				return imid;

			remaining--;
		}

		return imin;
	}








	// implementation:
	void Initialise()
	{
		MetadataAddr = GetPage();
		HeapAddress = GetPage();

		SizeOfHeap = 1;
		SizeOfMeta = 1;
		ChunksInHeap = 1;
		LastFree = 0;

		Chunk* c = index(0);
		c->offset = 0;
		c->size = 0x1000;
		setfree(c);
	}

	void CreateChunk(uint64_t offset, uint64_t size)
	{
		uint64_t o = bsearch(offset, [](uint64_t i) -> uint64_t { return index(i)->offset; });
		ChunksInHeap++;

		// create chunk in offset-sorted metalist
		{
			if(o <= ChunksInHeap)
			{
				// check if there are more chunks behind us.
				if(o < ChunksInHeap - 1)
					pushback(o);

				// now that's solved, make the chunk.
				Chunk* c = index(o);
				c->offset = offset;
				c->size = size;
				setfree(c);
			}
			else
				fail();
		}
	}


	void ExpandHeap()
	{
		// expand the heap.
		GetPageFixed(HeapAddress + (SizeOfHeap * 0x1000));

		// always offset sorted;
		Chunk* last = index(ChunksInHeap - 1);
		if(last->offset + size(last) != (SizeOfHeap * 0x1000))
		{
			fprintf(stderr, "failure (1): %lx + %lx != %lx\n", last->offset, size(last), SizeOfHeap * 0x1000);
			fail();
		}


		// either create a new chunk, or expand the last one.
		if(isfree(last))
		{
			last->size += 0x1000;
			setfree(last);
		}
		else
		{
			CreateChunk(SizeOfHeap * 0x1000, 0x1000);
		}
		SizeOfHeap++;
	}



	void* Allocate(size_t sz)
	{
		sz = round(sz);
		// loop through each chunk, hoping to find something big enough.

		Chunk* c = 0;
		for(uint64_t i = LastFree; i < ChunksInHeap; i++)
		{
			if(size(index(i)) >= sz && isfree(index(i)))
			{
				c = index(i);
				break;
			}
		}
		if(c == 0)
		{
			ExpandHeap();
			return Allocate(sz);
		}

		uint64_t o = c->offset;
		uint64_t oldsize = size(c);

		c->size = sz;
		setused(c);

		auto newsize = oldsize - sz;
		if(newsize >= Alignment)
		{
			CreateChunk(c->offset + sz, newsize - (newsize % Alignment));
		}
		else
		{
			c->size = oldsize;
			setused(c);
		}

		assert(sz % Alignment == 0);
		assert(o % Alignment == 0);
		return (void*) (HeapAddress + o);
	}

	void Free(void* ptr)
	{
		uint64_t p = (uint64_t) ptr;
		assert(p >= HeapAddress);
		p -= HeapAddress;

		// this is where the offset-sorted list comes in handy.
		uint64_t o = bsearch(p, [](uint64_t i) -> uint64_t { return index(i)->offset; });
		Chunk* self = index(o);


		if(self->offset != p)
		{
			fprintf(stderr, "failure: got offset %lx, expected %lx", p, self->offset);
			fail();
		}

		setfree(self);

		// do merge here.
		// check right first, because checking left may modify our own state.

		if(o < ChunksInHeap - 1)
		{
			// check right neighbour
			Chunk* right = index(o + 1);
			assert(self->offset + size(self) == right->offset);
			if(isfree(right))
			{
				self->size += size(right);
				setfree(self);

				pullfront(o + 1);
				ChunksInHeap--;
			}
		}
		if(o > 0)
		{
			// check left neighbour
			Chunk* left = index(o - 1);
			if(left->offset + size(left) != self->offset)
			{
				fprintf(stderr, "failure (2): %lx + %lx != %lx\n", left->offset, size(left), self->offset);
				fail();
			}

			if(isfree(left))
			{
				// do merge
				left->size += size(self);
				setfree(left);

				// delete us.
				pullfront(o);
				ChunksInHeap--;
			}
		}
	}

	void* Reallocate(void* ptr, size_t newsize)
	{
		// get a new chunk.
		if(newsize == 0)
		{
			Free(ptr);
			return NULL;
		}
		else
		{
			uint64_t p = (uint64_t) ptr;
			assert(p >= HeapAddress);
			p -= HeapAddress;

			// this is where the offset-sorted list comes in handy.
			uint64_t o = bsearch(p, [](uint64_t i) -> uint64_t { return index(i)->offset; });
			Chunk* self = index(o);

			if(self->offset != p)
			{
				fprintf(stderr, "failure: got offset %lx, expected %lx", p, self->offset);
				fail();
			}

			// 'self' is now valid.
			// get the offset.
			auto s = size(self);
			void* newc = Allocate(newsize);
			memcpy(newc, ptr, newsize > s ? s : newsize);

			Free(ptr);
			return newc;
		}
	}

	void Print()
	{
		for(long i = ChunksInHeap - 1; i >= 0; i--)
		{
			Chunk* c = index(i);
			printf("[%d]: (%#x, %#x) - %d\n", i, c->offset, size(c), isfree(c));
		}
	}
}




















