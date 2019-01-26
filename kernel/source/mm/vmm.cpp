// vmm.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace vmm
{
	using namespace addrs;

	// we have one extmm for each address space we can allocate.
	static extmm::State extmmState[NumAddressSpaces];
	static const char* extmmNames[NumAddressSpaces] = {
		"vmm/user",
		"vmm/heap",
		"vmm/kernel"
	};

	static addr_t end(addr_t base, size_t num)  { return base + (num * PAGE_SIZE); }


	void init(scheduler::Process* proc)
	{
		memset(extmmState, 0, sizeof(extmm::State) * NumAddressSpaces);

		for(size_t i = 0; i < NumAddressSpaces; i++)
		{
			auto s = &extmmState[i];

			mapAddress(VMMStackAddresses[i][0], pmm::allocate(1), 1, PAGE_PRESENT);
			extmm::init(s, extmmNames[i], VMMStackAddresses[i][0], VMMStackAddresses[i][1]);

			extmm::deallocate(s, AddressSpaces[i][0], (AddressSpaces[i][1] - AddressSpaces[i][0]) / PAGE_SIZE);
		}

		// unmap the null page.
		unmapAddress(0, 1, /* freePhys: */ false);

		log("vmm", "initialised with 3 addr spaces");
	}


	// these are the address spaces that we can hand out:
	// USER_ADDRSPACE_BASE - USER_ADDRSPACE_END
    // KERNEL_HEAP_BASE - KERNEL_HEAP_END
	// KERNEL_VMM_ADDRSPACE_BASE - KERNEL_VMM_ADDRSPACE_END

	addr_t allocateAddrSpace(size_t num, AddressSpace type)
	{
		extmm::State* st = 0;
		if(type == AddressSpace::User)              st = &extmmState[0];
		else if(type == AddressSpace::KernelHeap)   st = &extmmState[1];
		else if(type == AddressSpace::Kernel)       st = &extmmState[2];
		else                                        abort("allocateAddrSpace(): invalid address space '%d'!", type);

		return extmm::allocate(st, num, [](addr_t, size_t) -> bool { return true; });
	}

	void deallocateAddrSpace(addr_t addr, size_t num)
	{
		extmm::State* st = 0;
		for(size_t i = 0; i < NumAddressSpaces; i++)
		{
			if(addr >= AddressSpaces[i][0] && end(addr, num) <= AddressSpaces[i][1])
			{
				st = &extmmState[i];
				break;
			}
		}

		if(!st) abort("deallocateAddrSpace(): address not in any of the address spaces!");

		return extmm::deallocate(st, addr, num);
	}

	addr_t allocateSpecific(addr_t addr, size_t num)
	{
		extmm::State* st = 0;
		for(size_t i = 0; i < NumAddressSpaces; i++)
		{
			if(addr >= AddressSpaces[i][0] && end(addr, num) <= AddressSpaces[i][1])
			{
				st = &extmmState[i];
				break;
			}
		}

		if(!st) abort("allocateSpecific(): no address space to allocate address '%p'", addr);

		return extmm::allocateSpecific(st, addr, num);
	}




	addr_t allocate(size_t num, AddressSpace type)
	{
		auto virt = allocateAddrSpace(num, type);
		if(virt == 0) return 0;

		auto phys = pmm::allocate(num);
		if(phys == 0) return 0;

		mapAddress(virt, phys, num, PAGE_PRESENT);
		return virt;
	}

	void deallocate(addr_t addr, size_t num)
	{
		unmapAddress(addr, num, /* freePhys: */ true);
		deallocateAddrSpace(addr, num);
	}
}
}






















