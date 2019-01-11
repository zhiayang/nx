// vmm.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace vmm
{
	using namespace addrs;


	static addr_t physPML4Address = 0;

	// we have one extmm for each address space we can allocate.
	static extmm::State extmmState[3];

	static addr_t end(addr_t base, size_t num)  { return base + (num * PAGE_SIZE); }


	void init(BootInfo* bootinfo)
	{
		// this is physical!
		physPML4Address = bootinfo->pml4Address;

		memset(extmmState, 0, sizeof(extmm::State) * 3);

		mapAddress(VMM_STACK0_BASE, pmm::allocate(1), 1, PAGE_PRESENT | PAGE_WRITE);
		mapAddress(VMM_STACK1_BASE, pmm::allocate(1), 1, PAGE_PRESENT | PAGE_WRITE);
		mapAddress(VMM_STACK2_BASE, pmm::allocate(1), 1, PAGE_PRESENT | PAGE_WRITE);

		extmm::init(&extmmState[0], "vmm/user", VMM_STACK0_BASE, VMM_STACK0_END);
		extmm::init(&extmmState[1], "vmm/heap", VMM_STACK1_BASE, VMM_STACK1_END);
		extmm::init(&extmmState[2], "vmm/kernel", VMM_STACK2_BASE, VMM_STACK2_END);

		// add some extents, but by deallocating them.
		// 0 is user, 1 is kernel_heap, 2 is kernel_general.
		extmm::deallocate(&extmmState[0], USER_ADDRSPACE_BASE,
			(USER_ADDRSPACE_END - USER_ADDRSPACE_BASE) / PAGE_SIZE);

		extmm::deallocate(&extmmState[1], KERNEL_HEAP_BASE,
			(KERNEL_HEAP_END - KERNEL_HEAP_BASE) / PAGE_SIZE);

		extmm::deallocate(&extmmState[2], KERNEL_VMM_ADDRSPACE_BASE,
			(KERNEL_VMM_ADDRSPACE_END - KERNEL_VMM_ADDRSPACE_BASE) / PAGE_SIZE);

		// unmap the null page.
		unmapAddress(0, 1);

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

		if(addr >= USER_ADDRSPACE_BASE && end(addr, num) <= USER_ADDRSPACE_END)
			st = &extmmState[0];

		else if(addr >= KERNEL_HEAP_BASE && end(addr, num) <= KERNEL_HEAP_END)
			st = &extmmState[1];

		else if(addr >= KERNEL_VMM_ADDRSPACE_BASE && end(addr, num) <= KERNEL_VMM_ADDRSPACE_END)
			st = &extmmState[2];

		else
			abort("deallocateAddrSpace(): address not in any of the address spaces!");

		return extmm::deallocate(st, addr, num);
	}

	addr_t allocateSpecific(addr_t addr, size_t num)
	{
		extmm::State* st = 0;
		if(addr >= USER_ADDRSPACE_BASE && addr < USER_ADDRSPACE_END)                    st = &extmmState[0];
		else if(addr >= KERNEL_HEAP_BASE && addr < KERNEL_HEAP_END)                     st = &extmmState[1];
		else if(addr >= KERNEL_VMM_ADDRSPACE_BASE && addr < KERNEL_VMM_ADDRSPACE_END)   st = &extmmState[2];
		else abort("allocateSpecific(): no address space to allocate address '%p'", addr);

		return extmm::allocateSpecific(st, addr, num);
	}




	addr_t allocate(size_t num, AddressSpace type)
	{
		auto virt = allocateAddrSpace(num, type);
		if(virt == 0) return 0;

		auto phys = pmm::allocate(num);
		if(phys == 0) return 0;

		mapAddress(virt, phys, num, PAGE_WRITE | PAGE_PRESENT);
		return virt;
	}

	void deallocate(addr_t addr, size_t num)
	{
		for(size_t i = 0; i < num; i++)
			pmm::deallocate(getPhysAddr(end(addr, i)), 1);

		unmapAddress(addr, num);
		deallocateAddrSpace(addr, num);
	}
}
}






















