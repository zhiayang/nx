// vmm.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "extmm.h"

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

	using ExtMMState = extmm::State<>;

	static ExtMMState* getAddrSpace(addr_t addr, size_t num, ExtMMState* states)
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
		bool isFirst = (proc == scheduler::getKernelProcess());
		memset(proc->addrspace.vmmStates, 0, sizeof(ExtMMState) * NumAddressSpaces);

		for(size_t i = 0; i < NumAddressSpaces; i++)
		{
			auto s = &proc->addrspace.vmmStates[i];

			if(isFirst)
			{
				mapAddress(VMMStackAddresses[i][0], pmm::allocate(1), 1, PAGE_WRITE | PAGE_PRESENT, proc);
				s->init(extmmNames[i], VMMStackAddresses[i][0], VMMStackAddresses[i][1]);
			}
			else
			{
				s->init(extmmNames[i], 0, 0);
			}

			s->deallocate(AddressSpaces[i][0], (AddressSpaces[i][1] - AddressSpaces[i][0]) / PAGE_SIZE);
		}

		// unmap the null page.
		if(isFirst)
		{
			unmapAddress(0, 1);
		}
		else
		{
			// setup the page tables for this guy.
			setupAddrSpace(proc);
		}

		// log("vmm", "initialised vmm for pid %lu (cr3: %p)", proc->processId, proc->cr3);
	}

	void destroy(scheduler::Process* proc)
	{
		assert(proc);
		assert(proc->processId != 0);       // yo wtf that's illegal

		for(size_t i = 0; i < NumAddressSpaces; i++)
			proc->addrspace.vmmStates[i].destroy();

		// note: we don't need to un-setupAddrSpace, because that doesn't
		// allocate any memory -- only address spaces.

		// log("vmm", "cleaned up vmm for pid %lu (cr3: %p)", proc->processId, proc->cr3);
	}



	// these are the address spaces that we can hand out:
	// USER_ADDRSPACE_BASE          ->      USER_ADDRSPACE_END
	// KERNEL_HEAP_BASE             ->      KERNEL_HEAP_END
	// KERNEL_VMM_ADDRSPACE_BASE    ->      KERNEL_VMM_ADDRSPACE_END

	addr_t allocateAddrSpace(size_t num, AddressSpaceType type, scheduler::Process* proc)
	{
		if(proc == 0) proc = scheduler::getCurrentProcess();
		assert(proc);

		ExtMMState* st = 0;
		if(type == AddressSpaceType::User)              st = &proc->addrspace.vmmStates[0];
		else if(type == AddressSpaceType::KernelHeap)   st = &proc->addrspace.vmmStates[1];
		else if(type == AddressSpaceType::Kernel)       st = &proc->addrspace.vmmStates[2];
		else                                        abort("allocateAddrSpace(): invalid address space '%d'!", type);

		auto ret = st->allocate(num, [](addr_t, size_t) -> bool { return true; });
		assert(isAligned(ret));

		return ret;
	}

	void deallocateAddrSpace(addr_t addr, size_t num, scheduler::Process* proc)
	{
		assert(isAligned(addr));

		if(proc == 0) proc = scheduler::getCurrentProcess();
		assert(proc);

		ExtMMState* st = getAddrSpace(addr, num, proc->addrspace.vmmStates);
		if(!st) abort("deallocateAddrSpace(): address not in any of the address spaces!");

		return st->deallocate(addr, num);
	}





	addr_t allocateSpecific(addr_t addr, size_t num, scheduler::Process* proc)
	{
		assert(isAligned(addr));

		if(proc == 0) proc = scheduler::getCurrentProcess();
		assert(proc);

		ExtMMState* st = getAddrSpace(addr, num, proc->addrspace.vmmStates);
		if(!st) abort("allocateSpecific(): no address space to allocate address '%p'", addr);

		auto virt = st->allocateSpecific(addr, num);
		if(virt) proc->addrspace.addRegion(virt, num);

		return virt;
	}

	addr_t allocate(size_t num, AddressSpaceType type, uint64_t flags, scheduler::Process* proc)
	{
		if(proc == 0) proc = scheduler::getCurrentProcess();
		assert(proc);

		auto virt = allocateAddrSpace(num, type, proc);
		if(virt == 0) { abort("vmm::allocate(): out of addrspace!"); return 0; }

		// don't have the write flag.
		flags &= ~PAGE_WRITE;

		mapLazy(virt, num, flags, proc);
		proc->addrspace.addRegion(virt, num);

		return virt;
	}

	addr_t allocateEager(size_t num, AddressSpaceType type, uint64_t flags, scheduler::Process* proc)
	{
		if(proc == 0) proc = scheduler::getCurrentProcess();
		assert(proc);

		auto virt = allocateAddrSpace(num, type, proc);
		if(virt == 0) { abort("vmm::allocate(): out of addrspace!"); return 0; }

		auto phys = pmm::allocate(num);
		if(phys == 0) { abort("vmm::allocate(): no physical pages!"); return 0; }

		mapAddress(virt, phys, num, flags | PAGE_PRESENT, proc);
		proc->addrspace.addRegion(virt, phys, num);

		return virt;
	}

	void deallocate(addr_t addr, size_t num, scheduler::Process* proc)
	{
		assert(isAligned(addr));

		if(proc == 0) proc = scheduler::getCurrentProcess();
		assert(proc);

		unmapAddress(addr, num, /* ignoreIfNotMapped: */ false, proc);
		deallocateAddrSpace(addr, num, proc);

		proc->addrspace.freeRegion(addr, num, /* freePhys: */ true);
	}
}
}






















