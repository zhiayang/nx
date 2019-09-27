// pmm.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "bfx.h"
#include "bootboot.h"

namespace bfx {
namespace extmm
{
	struct extent_t
	{
		uint64_t addr;
		uint64_t size;

		extent_t* next = 0;
	};

	struct State
	{
		extent_t* head = 0;
		size_t numExtents = 0;

		const char* owner = 0;

		uint64_t bootstrapStart = 0;
		uint64_t bootstrapWatermark = 0;
		uint64_t bootstrapEnd = 0;
	};

	static uint64_t end(uint64_t base, size_t num)  { return base + (num * PAGE_SIZE); }
	static uint64_t end(extent_t* ext)              { return ext->addr + (ext->size * PAGE_SIZE); }

	static void addExtent(State* st, uint64_t addr, size_t size)
	{
		extent_t* ext = 0;
		if(st->bootstrapWatermark + sizeof(extent_t) <= st->bootstrapEnd)
		{
			ext = (extent_t*) st->bootstrapWatermark;
			st->bootstrapWatermark += sizeof(extent_t);
		}
		else
		{
			// we're out of space, lmao.
			return;

			// ext = new extent_t();
		}

		ext->addr = addr;
		ext->size = size;

		// insert at the head.
		ext->next = st->head;

		st->head = ext;
		st->numExtents += 1;
	}

	void dump(State* st)
	{
		println("dumping extmm state %s (%p):", st->owner, st);
		auto ext = st->head;
		while(ext)
		{
			println("    %p - %p", ext->addr, end(ext));
			ext = ext->next;
		}
		println("");
	}


	void init(State* st, const char* owner, uint64_t base, uint64_t top)
	{
		st->head = 0;
		st->numExtents = 0;

		st->owner = owner;

		st->bootstrapWatermark = base;
		st->bootstrapStart = base;
		st->bootstrapEnd = top;
	}

	void destroy(State* st)
	{
		// just loop through all the extents and destroy them.
		auto ext = st->head;
		while(ext)
		{
			if(!(((uint64_t) ext) >= st->bootstrapStart && ((uint64_t) ext) < st->bootstrapEnd))
			{
				// delete ext;
			}

			// else do nothing.

			ext = ext->next;
		}

		st->head = 0;
		st->numExtents = 0;
		st->bootstrapWatermark = st->bootstrapStart;
	}


	uint64_t allocate(State* st, size_t num, bool (*satisfies)(uint64_t, size_t))
	{
		if(num == 0) abort("extmm/%s::allocate(): cannot allocate 0 pages!", st->owner);

		auto ext = st->head;
		while(ext)
		{
			if(ext->size >= num && satisfies(ext->addr, ext->size))
			{
				// return this.
				// take from the bottom.

				uint64_t ret = ext->addr;
				ext->addr += (num * PAGE_SIZE);
				ext->size -= num;

				return ret;
			}

			ext = ext->next;
		}

		println("extmm/%s::allocate(): out of pages!", st->owner);
		return 0;
	}

	uint64_t allocateSpecific(State* st, uint64_t start, size_t num)
	{
		auto ext = st->head;
		while(ext)
		{
			if(ext->addr <= start && end(ext) >= end(start, num))
			{
				if(ext->addr == start)
				{
					// simple -- just move the pointer up
					ext->addr += (num * PAGE_SIZE);
					ext->size -= num;
				}
				else if(end(start, num) == end(ext))
				{
					// also simple -- just subtract the size.
					ext->size -= num;
				}
				else
				{
					// bollocks, it's somewhere in the middle.
					size_t numFrontPages = (start - ext->addr) / PAGE_SIZE;
					size_t numBackPages = (end(ext) - end(start, num)) / PAGE_SIZE;

					// decrease the front block
					ext->size = numFrontPages;

					// make a new block
					addExtent(st, end(start, num), numBackPages);
				}

				return start;
			}

			ext = ext->next;
		}

		println("extmm/%s::allocateSpecific(): could not fulfil request!", st->owner);
		return 0;
	}


	void deallocate(State* st, uint64_t addr, size_t num)
	{
		// loop through every extent. we can match in two cases:
		// 1. 'addr' is 'num' pages *below* the base of the extent
		// 2. 'addr' is *size* pages above the *end* of the extent

		auto ext = st->head;
		while(ext)
		{
			if(end(addr, num) == ext->addr)
			{
				// we are below. decrease the addr of the extent by num pages.
				ext->addr -= (PAGE_SIZE * num);
				return;
			}
			else if(end(ext) == addr)
			{
				// increase the size only
				ext->size += num;
				return;
			}

			ext = ext->next;
		}

		// oops. make a new extent.
		addExtent(st, addr, num);
	}
}

namespace pmm
{
	// this is the number of pages that we'll use for our bootstrap of the memory manager.
	constexpr size_t NumReservedPages = 8;

	static uint64_t end(uint64_t base, size_t num)  { return base + (num * PAGE_SIZE); }

	static extmm::State extmmState;

	constexpr uintptr_t PMM_STACK_BASE  = 0xFFFF'FFFF'F3C0'0000;
	constexpr uintptr_t PMM_STACK_END   = 0xFFFF'FFFF'F3D0'0000;

	void init(MMapEnt* ents, size_t numEnts)
	{
		// find a bootstrappable chunk of memory marked as usable.
		bool bootstrapped = false;
		{
			for(size_t i = 0; i < numEnts; i++)
			{
				// idk what is going on, but using low memory fucks us up big time.
				if(MMapEnt_Type(&ents[i]) == MMAP_FREE && MMapEnt_Size(&ents[i]) / PAGE_SIZE >= NumReservedPages
					&& MMapEnt_Ptr(&ents[i]) < 0xFFFF'FFFF && MMapEnt_Ptr(&ents[i]) >= 0x100000)
				{
					// ok. modify the map first to steal the memory:
					uint64_t base = MMapEnt_Ptr(&ents[i]);

					ents[i].ptr += (NumReservedPages * PAGE_SIZE);
					ents[i].size -= (PAGE_SIZE * NumReservedPages);

					// bootstrap!
					vmm::bootstrap(base, PMM_STACK_BASE, NumReservedPages);
					bootstrapped = true;
					break;
				}
			}
		}

		if(!bootstrapped) abort("failed to bootstrap pmm!!");

		// now that we have bootstrapped one starting page for ourselves, we can init the ext mm.
		extmm::init(&extmmState, "pmm", PMM_STACK_BASE, PMM_STACK_END);

		// ok, now loop through each memory entry for real.
		size_t totalMem = 0;
		for(size_t i = 0; i < numEnts; i++)
		{
			auto entry = &ents[i];
			if(MMapEnt_Type(entry) == MMAP_FREE)
			{
				// println("%p - %p", MMapEnt_Ptr(entry), MMapEnt_Ptr(entry) + MMapEnt_Size(entry));

				deallocate(MMapEnt_Ptr(entry), MMapEnt_Size(entry) / PAGE_SIZE);
				totalMem += MMapEnt_Size(entry);
			}
		}

		println("bfx pmm initialised with %zu extents, %zu bytes", extmmState.numExtents, totalMem);
	}

	uint64_t allocate(size_t num, bool below4G)
	{
		// lol. we can't have capturing lambdas without some kind of std::function
		// and nobody knows how to implement that shit.
		return extmm::allocate(&extmmState, num, below4G ? [](uint64_t a, size_t l) -> bool {
			return end(a, l) < 0xFFFF'FFFF;
		} : [](uint64_t, size_t) -> bool { return true; });
	}

	void deallocate(uint64_t addr, size_t num)
	{
		extmm::deallocate(&extmmState, addr, num);
	}
}
}








