// vmm.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace vmm
{
	using namespace addrs;

	// we have one extmm for each address space we can allocate.
	static const char* extmmNames[NumAddressSpaces] = {
		"vmm/user",
		"vmm/heap",
		"vmm/kernel"
	};

	static addr_t end(addr_t base, size_t num)  { return base + (num * PAGE_SIZE); }

	static extmm::State* getAddrSpace(addr_t addr, size_t num, extmm::State* states)
	{
		for(size_t i = 0; i < NumAddressSpaces; i++)
		{
			if(addr >= AddressSpaces[i][0] && end(addr, num) <= AddressSpaces[i][1])
				return &states[i];
		}

		return 0;
	}


	void init(scheduler::Process* proc)
	{
		memset(proc->vmmStates, 0, sizeof(extmm::State) * NumAddressSpaces);

		for(size_t i = 0; i < NumAddressSpaces; i++)
		{
			auto s = &proc->vmmStates[i];

			mapAddress(VMMStackAddresses[i][0], pmm::allocate(1), 1, PAGE_PRESENT, proc);
			extmm::init(s, extmmNames[i], VMMStackAddresses[i][0], VMMStackAddresses[i][1]);

			extmm::deallocate(s, AddressSpaces[i][0], (AddressSpaces[i][1] - AddressSpaces[i][0]) / PAGE_SIZE);
		}


		// unmap the null page.
		if(proc == scheduler::getKernelProcess())
		{
			unmapAddress(0, 1, /* freePhys: */ false);
		}
		else
		{
			// setup the page tables for this guy.
			setupAddrSpace(proc);
		}

		log("vmm", "initialised vmm for pid %lu", proc->processId);
	}


	// these are the address spaces that we can hand out:
	// USER_ADDRSPACE_BASE          ->      USER_ADDRSPACE_END
    // KERNEL_HEAP_BASE             ->      KERNEL_HEAP_END
	// KERNEL_VMM_ADDRSPACE_BASE    ->      KERNEL_VMM_ADDRSPACE_END

	addr_t allocateAddrSpace(size_t num, AddressSpace type, scheduler::Process* proc)
	{
		if(proc == 0) proc = scheduler::getCurrentProcess();
		assert(proc);

		extmm::State* st = 0;
		if(type == AddressSpace::User)              st = &proc->vmmStates[0];
		else if(type == AddressSpace::KernelHeap)   st = &proc->vmmStates[1];
		else if(type == AddressSpace::Kernel)       st = &proc->vmmStates[2];
		else                                        abort("allocateAddrSpace(): invalid address space '%d'!", type);

		auto ret = extmm::allocate(st, num, [](addr_t, size_t) -> bool { return true; });
		serial::debugprintf("alloc vmm (%p)  %p - %p\n", proc->cr3, ret, end(ret, num));

		return ret;
	}

	void deallocateAddrSpace(addr_t addr, size_t num, scheduler::Process* proc)
	{
		if(proc == 0) proc = scheduler::getCurrentProcess();
		assert(proc);

		extmm::State* st = getAddrSpace(addr, num, proc->vmmStates);
		if(!st) abort("deallocateAddrSpace(): address not in any of the address spaces!");

		serial::debugprintf("free vmm (%p)  %p - %p\n", proc->cr3, addr, end(addr, num));

		return extmm::deallocate(st, addr, num);
	}





	addr_t allocateSpecific(addr_t addr, size_t num, scheduler::Process* proc)
	{
		if(proc == 0) proc = scheduler::getCurrentProcess();
		assert(proc);

		extmm::State* st = getAddrSpace(addr, num, proc->vmmStates);
		if(!st) abort("allocateSpecific(): no address space to allocate address '%p'", addr);

		return extmm::allocateSpecific(st, addr, num);
	}

	addr_t allocate(size_t num, AddressSpace type, scheduler::Process* proc)
	{
		if(proc == 0) proc = scheduler::getCurrentProcess();
		assert(proc);

		auto virt = allocateAddrSpace(num, type, proc);
		if(virt == 0) return 0;

		auto phys = pmm::allocate(num);
		if(phys == 0) return 0;

		mapAddress(virt, phys, num, PAGE_PRESENT, proc);
		return virt;
	}

	void deallocate(addr_t addr, size_t num, scheduler::Process* proc)
	{
		if(proc == 0) proc = scheduler::getCurrentProcess();
		assert(proc);

		unmapAddress(addr, num, /* freePhys: */ true, proc);
		deallocateAddrSpace(addr, num, proc);
	}
}
}






















