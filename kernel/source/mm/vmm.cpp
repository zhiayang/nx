// vmm.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace vmm
{
	static addr_t physPML4Address = 0;

	// we have one extmm for each address space we can allocate.
	static extmm::State extmmState[3];

	static addr_t end(addr_t base, size_t num)  { return base + (num * PAGE_SIZE); }


	void init(BootInfo* bootinfo)
	{
		// this is physical!
		physPML4Address = bootinfo->pml4Address;

		memset(extmmState, 0, sizeof(extmm::State) * 3);

		mapAddress(addrs::VMM_STACK0_BASE, pmm::allocate(1), 1, PAGE_PRESENT | PAGE_WRITE);
		mapAddress(addrs::VMM_STACK1_BASE, pmm::allocate(1), 1, PAGE_PRESENT | PAGE_WRITE);
		mapAddress(addrs::VMM_STACK2_BASE, pmm::allocate(1), 1, PAGE_PRESENT | PAGE_WRITE);

		extmm::init(&extmmState[0], addrs::VMM_STACK0_BASE, addrs::VMM_STACK0_END);
		extmm::init(&extmmState[1], addrs::VMM_STACK1_BASE, addrs::VMM_STACK1_END);
		extmm::init(&extmmState[2], addrs::VMM_STACK2_BASE, addrs::VMM_STACK2_END);

		// add some extents, but by deallocating them.
		// 0 is user, 1 is kernel_heap, 2 is kernel_general.
		extmm::deallocate(&extmmState[0], addrs::USER_ADDRSPACE_BASE,
			(addrs::USER_ADDRSPACE_END - addrs::USER_ADDRSPACE_BASE) / PAGE_SIZE);

		extmm::deallocate(&extmmState[1], addrs::KERNEL_HEAP_BASE,
			(addrs::KERNEL_HEAP_END - addrs::KERNEL_HEAP_BASE) / PAGE_SIZE);

		extmm::deallocate(&extmmState[2], addrs::KERNEL_VMM_ADDRSPACE_BASE,
			(addrs::KERNEL_VMM_ADDRSPACE_END - addrs::KERNEL_VMM_ADDRSPACE_BASE) / PAGE_SIZE);

		println("vmm initialised");
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

		if(addr >= addrs::USER_ADDRSPACE_BASE && end(addr, num) <= addrs::USER_ADDRSPACE_END)
			st = &extmmState[0];

		else if(addr >= addrs::KERNEL_HEAP_BASE && end(addr, num) <= addrs::KERNEL_HEAP_END)
			st = &extmmState[1];

		else if(addr >= addrs::KERNEL_VMM_ADDRSPACE_BASE && end(addr, num) <= addrs::KERNEL_VMM_ADDRSPACE_END)
			st = &extmmState[2];

		else
			abort("deallocateAddrSpace(): address not in any of the address spaces!");

		return extmm::deallocate(st, addr, num);
	}


	addr_t allocate(size_t num, AddressSpace type)
	{
		auto addr = allocateAddrSpace(num, type);
		if(addr == 0) return 0;

		mapAddress(addr, pmm::allocate(num), num, PAGE_WRITE | PAGE_PRESENT);
		return addr;
	}

	void deallocate(addr_t addr, size_t num)
	{
		deallocateAddrSpace(addr, num);
		for(size_t i = 0; i < num; i++)
			pmm::deallocate(getPhysAddr(end(addr, i)), 1);

		unmapAddress(addr, num);
	}
}
}






















