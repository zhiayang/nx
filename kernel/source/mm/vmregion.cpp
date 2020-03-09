// vmregion.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace vmm
{
	VMRegion::VMRegion(addr_t addr, size_t num) : addr(addr), numPages(num)
	{
		// make sure we reserve exactly that amount.
		this->backingPhysPages.resize(num);

		assert(this->backingPhysPages.size() == num);

		// aight. donded.
	}

	void AddressSpace::init(addr_t cr3)
	{
		if(cr3) this->cr3 = cr3;
		else    this->cr3 = pmm::allocate(1);
	}

	void AddressSpace::destroy()
	{
		size_t num = 1;
		pmm::deallocate(this->cr3, 1);

		for(auto x : this->allocatedPhysPages)
			num++, pmm::deallocate(x, 1);

		// destroy each region.
		for(const auto& r : this->regions)
			for(auto p : r.backingPhysPages)
				if(p) num++, pmm::deallocate(p, 1);

		log("adrspc", "freed %zu physical pages", num);
	}


	void AddressSpace::addRegion(addr_t addr, size_t num)
	{
		auto end = [](auto a, auto n) -> auto {
			return a + PAGE_SIZE * n;
		};

		// i'm sure we can find a way to use extmm for this, but i'm lazy
		for(auto& r : this->regions)
		{
			if(end(r.addr, r.numPages) == r.addr)
			{
				// we are below. decrease the addr of the extent by num pages.
				r.addr -= (PAGE_SIZE * num);
			}
			else if(end(r.addr, r.numPages) == addr)
			{
				// increase the size only
				r.numPages += num;
				return;
			}
			else if(r.addr == addr)
			{
				// hmm...
				abort("addrspace: potential double free! addr: %p, size: %zu", addr, num);
			}
		}

		this->regions.emplaceBack(addr, num);
	}

	void AddressSpace::freeRegion(addr_t addr, size_t size)
	{
	}
}
}

















