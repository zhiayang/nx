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

		extent_t* next = 0;
	};

	static addr_t end(addr_t base, size_t num)  { return base + (num * PAGE_SIZE); }
	static addr_t end(extent_t* ext)            { return ext->addr + (ext->size * PAGE_SIZE); }

	static void addExtent(State* st, addr_t addr, size_t size)
	{
		extent_t* ext = 0;
		if(st->bootstrapWatermark + sizeof(extent_t) <= st->bootstrapEnd)
		{
			ext = (extent_t*) st->bootstrapWatermark;
			st->bootstrapWatermark += sizeof(extent_t);
		}
		else
		{
			ext = new extent_t();
		}

		assert(ext);
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


	void init(State* st, const char* owner, addr_t base, addr_t top)
	{
		st->head = 0;
		st->numExtents = 0;

		st->owner = owner;

		st->bootstrapWatermark = base;
		st->bootstrapEnd = top;
	}


	addr_t allocate(State* st, size_t num, bool (*satisfies)(addr_t, size_t))
	{
		if(num == 0) abort("extmm/%s::allocate(): cannot allocate 0 pages!", st->owner);

		auto ext = st->head;
		while(ext)
		{
			if(ext->size >= num && satisfies(ext->addr, ext->size))
			{
				// return this.
				// take from the bottom.

				addr_t ret = ext->addr;
				ext->addr += (num * PAGE_SIZE);
				ext->size -= num;

				return ret;
			}

			ext = ext->next;
		}

		println("extmm/%s::allocate(): out of pages!", st->owner);
		return 0;
	}

	addr_t allocateSpecific(State* st, addr_t start, size_t num)
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


	void deallocate(State* st, addr_t addr, size_t num)
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
}















