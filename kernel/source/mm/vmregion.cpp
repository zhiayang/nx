// vmregion.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace vmm
{
	void AddressSpace::init(addr_t cr3)
	{
		if(cr3) this->cr3 = cr3;
		else    this->cr3 = pmm::allocate(1);
	}

	void AddressSpace::destroy()
	{
		pmm::deallocate(this->cr3, 1);

		for(auto x : this->allocatedPhysPages)
			pmm::deallocate(x, 1);

		log("adrspc", "freed %zu physical pages", 1 + this->allocatedPhysPages.size());
	}
}
}
