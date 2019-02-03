// extent_alloc.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.


// this is an abstracted form of the 'extent' system that we use for our page allocators.

#include "nx.h"

namespace nx {
namespace extmm
{
	struct extent_t
	{
		uint64_t addr;
		uint64_t size;
	};

	static constexpr size_t extentsPerPage      = PAGE_SIZE / sizeof(extent_t);

	static addr_t end(addr_t base, size_t num)  { return base + (num * PAGE_SIZE); }
	static addr_t end(extent_t* ext)            { return ext->addr + (ext->size * PAGE_SIZE); }


	static void extendExtentArray(State* st)
	{
		auto virt = (addr_t) st->extents + (st->numPages * PAGE_SIZE);
		if(virt == st->maxAddress)
			abort("1 extmm/%s::extend(): reached expansion limit!", st->owner);

		auto phys = pmm::allocate(1);

		vmm::mapAddress(virt, phys, 1, vmm::PAGE_PRESENT);
	}

	static void addExtent(State* st, addr_t addr, size_t size)
	{
		extent_t ext;
		ext.addr = addr;
		ext.size = size;

		st->extents[st->numExtents] = ext;
		st->numExtents++;

		if(st->numExtents % extentsPerPage == 0)
			extendExtentArray(st);
	}





	void init(State* st, const char* owner, addr_t base, addr_t top)
	{
		st->extents = (extent_t*) base;

		st->numPages = 1;
		st->numExtents = 0;

		st->maxAddress = top;
		st->owner = owner;
	}


	addr_t allocate(State* st, size_t num, bool (*satisfies)(addr_t, size_t))
	{
		if(num == 0) abort("2 extmm/%s::allocate(): cannot allocate 0 pages!", st->owner);

		for(size_t i = 0; i < st->numExtents; i++)
		{
			auto ext = &st->extents[i];
			if(ext->size >= num && satisfies(ext->addr, ext->size))
			{
				// return this.
				// take from the bottom.

				addr_t ret = ext->addr;
				ext->addr += (num * PAGE_SIZE);
				ext->size -= num;

				return ret;
			}
		}

		println("3 extmm/%s::allocate(): out of pages!", st->owner);
		return 0;
	}

	addr_t allocateSpecific(State* st, addr_t start, size_t num)
	{
		for(size_t i = 0; i < st->numExtents; i++)
		{
			auto ext = &st->extents[i];
			if(ext->addr <= start && ext->size >= num)
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
					ext->size -= numFrontPages;

					// make a new block
					deallocate(st, end(start, num), numBackPages);
				}

				return start;
			}
		}

		println("4 extmm/%s::allocateSpecific(): could not fulfil request!", st->owner);
		return 0;
	}


	void deallocate(State* st, addr_t addr, size_t num)
	{
		// loop through every extent. we can match in two cases:
		// 1. 'addr' is 'num' pages *below* the base of the extent
		// 2. 'addr' is *size* pages above the *base* of the extent

		for(size_t i = 0; i < st->numExtents; i++)
		{
			auto ext = &st->extents[i];
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
		}

		// oops. make a new extent.
		addExtent(st, addr, num);
	}

}
}















