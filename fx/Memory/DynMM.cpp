// KernelHeap.cpp
// Copyright (c) 2013 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

// Heap things.

#include <stdint.h>
#include "../FxLoader.hpp"

#define MaximumHeapSizeInPages		0xFFFFFFFF
#define Alignment					32
#define MetadataSize				32
#define Warn						1
#define MapFlags					0x7
#define ClearMemory					1

static uint64_t NumberOfChunksInHeap;
static uint64_t SizeOfHeapInPages;
static uint64_t SizeOfMetadataInPages;
static uint64_t FirstFreeIndex;

static uint64_t FirstHeapMetadataPhysPage;
static uint64_t FirstHeapPhysPage;

#define HeapMetadata		0xFFFFFF1000000000
#define HeapAddress			0xFFFFFF2000000000

namespace Memory
{
	static bool DidInitialise = false;

	struct Block
	{
		uint64_t Offset;
		uint64_t Size;
		uint64_t callerAddr;
		uint64_t magic;
	};

	static uint64_t GetSize(Block* c)
	{
		uint64_t f = c->Size & (uint64_t)(~0x1);
		return f;
	}

	static bool IsFree(Block* c)
	{
		return c->Size & 0x1;
	}

	static void ExpandMetadata()
	{
		uint64_t d = HeapMetadata + (SizeOfMetadataInPages * 0x1000);
		uint64_t p = Memory::AllocatePage();

		Memory::MapAddress(d, p, MapFlags);
		SizeOfMetadataInPages++;
	}

	static Block* sane(Block* c)
	{
		if(c && (c->Offset ^ c->Size) == c->magic)
		{
			return c;
		}
		else
		{
			return c;
		}
	}

	static uint64_t CreateNewChunk(uint64_t offset, uint64_t size)
	{
		uint64_t s = HeapMetadata + MetadataSize;
		bool found = false;

		for(uint64_t m = 0; m < NumberOfChunksInHeap; m++)
		{
			Block* ct = (Block*) s;
			if((ct->Offset + GetSize(ct) == offset && IsFree(ct)) || (ct->Offset == 0 && ct->Size == 0))
			{
				found = true;
				break;
			}

			s += sizeof(Block);
		}

		if(found)
		{
			Block* c1 = (Block*) s;

			if(c1->Offset == 0 && c1->Size == 0)
			{
				// this is one of those derelict chunks, repurpose it.
				c1->Offset = offset;
				c1->Size = size & (uint64_t)(~0x1);
				c1->Size |= 0x1;
			}
			else
			{
				c1->Size += size;
				c1->Size |= 0x1;
			}

			c1->magic = c1->Size ^ c1->Offset;
			return (uint64_t) sane(c1);
		}




		uint64_t p = MetadataSize + ((NumberOfChunksInHeap - 1) * sizeof(Block));

		if(p + sizeof(Block) == SizeOfMetadataInPages * 0x1000)
		{
			// if this is the last one before the page
			// create a new one.
			ExpandMetadata();
		}

		// create another one.
		Block* c1 = (Block*) (HeapMetadata + p + sizeof(Block));
		NumberOfChunksInHeap++;

		c1->Offset = offset;
		c1->Size = size | 0x1;
		c1->magic = c1->Size ^ c1->Offset;

		return (uint64_t) sane(c1);
	}

	static uint64_t RoundSize(uint64_t s)
	{
		uint64_t remainder = s % Alignment;

		if(remainder == 0)
			return s;

		return s + Alignment - remainder;
	}


	void Initialise()
	{
		// the first page will contain bits of metadata.
		// including: number of chunks.
		// size of heap in pages.

		// let's start.
		// allocate one page for the heap.
		// will be mapped for now.
		FirstHeapMetadataPhysPage = Memory::AllocatePage();
		FirstHeapPhysPage = Memory::AllocatePage();

		// also map it.
		Memory::MapAddress(HeapMetadata, FirstHeapMetadataPhysPage, MapFlags);
		Memory::MapAddress(HeapAddress, FirstHeapPhysPage, MapFlags);

		NumberOfChunksInHeap = 1;
		SizeOfHeapInPages = 1;
		SizeOfMetadataInPages = 1;
		FirstFreeIndex = 0;


		Block* fc = (Block*) (HeapMetadata + MetadataSize);
		fc->Offset = 0;

		// bit zero stores free/not: 1 is free, 0 is not. that means we can only do 2-byte granularity.
		fc->Size = 0x1000 | 0x1;
		fc->magic = fc->Size ^ fc->Offset;

		DidInitialise = true;
	}

	static uint64_t FindFirstFreeSlot(uint64_t Size)
	{
		// free chunks are more often than not found at the back,
		{
			uint64_t p = HeapMetadata + MetadataSize + (FirstFreeIndex * sizeof(Block));

			for(uint64_t k = 0; k < NumberOfChunksInHeap - FirstFreeIndex; k++)
			{
				Block* c = sane((Block*) p);
				if(GetSize(c) > Size && IsFree(c))
					return p;

				else
					p += sizeof(Block);
			}

			// because the minimum offset (from the meta address) is 8, zero will do.
			return 0;
		}
	}


	static void SplitChunk(Block* Header, uint64_t Size)
	{
		// round up to nearest two.
		uint64_t ns = ((Size / 2) + 1) * 2;

		assert(GetSize(Header) >= ns);

		// create a new chunk.
		// because of the inherently unordered nature of the list (ie. we can't insert arbitrarily into the middle of the
		// list, for it is a simple array), we cannot make assumptions (aka assert() s) about the Offset and Size fields.
		// because this function will definitely disorder the list, because CreateNewChunk() can only put
		// new chunks at the end of the list.

		uint64_t nsonc = GetSize(Header) - ns;
		CreateNewChunk(Header->Offset + ns, nsonc);

		Header->Size = ns | 0x1;
		Header->magic = Header->Size ^ Header->Offset;
	}


	static void ExpandHeap()
	{
		uint64_t p = HeapMetadata + MetadataSize;
		bool found = false;
		for(uint64_t m = 0; m < NumberOfChunksInHeap; m++)
		{
			Block* ct = (Block*) p;
			if(ct->Offset + GetSize(ct) == SizeOfHeapInPages * 0x1000 && IsFree(ct))
			{
				found = true;
				break;
			}

			p += sizeof(Block);
		}


		// check if the chunk before the new one is free.
		if(found)
		{
			Block* c1 = (Block*) p;

			// merge with that instead.
			c1->Size += 0x1000;
			c1->Size |= 0x1;
			c1->magic = c1->Offset ^ c1->Size;
		}
		else
		{
			// nothing we can do.
			// create a new chunk.
			CreateNewChunk(SizeOfHeapInPages * 0x1000, 0x1000);
		}


		// create and map the actual space.
		uint64_t x = Memory::AllocatePage();
		Memory::MapAddress(HeapAddress + SizeOfHeapInPages * 0x1000, x, MapFlags);

		SizeOfHeapInPages++;
	}

	extern "C" void* malloc(uint64_t s)
	{
		uint64_t Size = (uint64_t) s;

		// round to alignment.
		uint64_t as = RoundSize(Size);

		if(as == 0)
			return nullptr;

		// find first matching chunk.
		uint64_t in = FindFirstFreeSlot(as);

		// check if we didn't find one.
		if(in == 0)
		{
			ExpandHeap();
			return malloc(as);
		}
		else
		{
			// we must have found it.
			// check that it's actually valid.
			assert(!(in % sizeof(Block)));

			Block* c = sane((Block*) in);

			if(GetSize(c) > as)
			{
				SplitChunk(c, as);
			}

			c->Size &= (uint64_t) (~0x1);
			c->magic = c->Offset ^ c->Size;
			c->callerAddr = (uint64_t) __builtin_return_address(1);	// two levels up, since (0) is probably operator new

			if(c->callerAddr == 0)	// but if it's 0, then... try (0).
				c->callerAddr = (uint64_t) __builtin_return_address(0);

			if(ClearMemory)
				memset((void*) (HeapAddress + c->Offset), 0, GetSize(c));

			return (void*) (c->Offset + HeapAddress);
		}
	}

	static Block* LookupBlockFromOffset(uint64_t offset)
	{
		// find the corresponding chunk in the mdata.
		uint64_t p = HeapMetadata + MetadataSize;
		Block* tc = 0;

		for(uint64_t k = 0; k < NumberOfChunksInHeap; k++)
		{
			Block* c = (Block*) p;
			if(offset - HeapAddress == (uint64_t) c->Offset)
			{
				if(k < FirstFreeIndex && c && IsFree(c))
					FirstFreeIndex = k;

				tc = c;
				break;
			}

			else
				p += sizeof(Block);
		}

		if(!tc)
		{
			return 0;
		}

		sane(tc);
		return tc;
	}


	extern "C" void free(void* Pointer)
	{
		// else, set it to free.
		Block* tc = LookupBlockFromOffset((uint64_t) Pointer);
		if(tc)
		{
			tc->Size |= 0x1;
			tc->magic = tc->Offset ^ tc->Size;
			memset((void*) ((uint64_t) (tc->Offset + HeapAddress)), 0x00, GetSize(tc));
		}
	}

	extern "C" void* realloc(void* ptr, uint64_t size)
	{
		Block* b = LookupBlockFromOffset((uint64_t) ptr);

		if(b->Size > size)
		{
			return (void*) b->Offset;
		}
		else
		{
			void* ret = malloc(size);
			assert(ret);

			memcpy(ret, ptr, b->Size);
			return ret;
		}
	}
}

