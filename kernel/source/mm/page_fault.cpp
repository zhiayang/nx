// page_fault.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace vmm
{
	bool handlePageFault(uint64_t cr2, uint64_t errorCode, uint64_t rip)
	{
		// The error code gives us details of what happened.
		uint8_t notPresent          = !(errorCode & 0x1); // Page not present
		uint8_t reservedBits        = errorCode & 0x8;    // Overwritten CPU-reserved bits of page entry?

		// if you (we?) overwrote reserved bits, or the page was already present
		// to begin with, then this isn't a lazy-alloc situation.
		if(reservedBits || !notPresent)
			return false;

		cr2 = vmm::PAGE_ALIGN(cr2);
		log("pf", "proc %lu #PF (cr2=%p, ip=%p)", scheduler::getCurrentProcess()->processId, cr2, rip);

		// this will get the flags for the current process's address space, so we
		// don't need to pass it explicitly.
		auto flags = vmm::getPageFlags(cr2);

		// if you dun goofed, then we can't help you
		if(!(flags & PAGE_PRESENT) && (flags & PAGE_LAZY_ALLOC))
		{
			// ok, we can do stuff here.
			// set the flags. this uses the current proc also.
			auto phys = pmm::allocate(1);

			log("pf", "allocated phys %p to virt %p", phys, cr2);

			vmm::mapAddress(cr2, phys, 1, (flags & ~PAGE_LAZY_ALLOC));
			return true;
		}
		else
		{
			// u dun goofed
			return false;
		}
	}
}
}






