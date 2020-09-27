// vmm.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "extmm.h"

namespace nx {
namespace vmm
{
	using namespace addrs;

	constexpr VirtAddr AddressSpaces[NumAddressSpaces][2] = {
		{ addrs::KERNEL_VMM_ADDRSPACE_BASE, addrs::KERNEL_VMM_ADDRSPACE_END },
		{ addrs::KERNEL_HEAP_BASE,          addrs::KERNEL_HEAP_END          },
		{ addrs::USER_ADDRSPACE_BASE,       addrs::USER_ADDRSPACE_END       }
	};

	// we have one extmm for each address space we can allocate.
	static const char* extmmNames[NumAddressSpaces] = {
		"vmm/user",
		"vmm/heap",
		"vmm/kernel"
	};


	using ExtMMState = extmm::State<>;

	static AddressSpaceType getAddrSpace(VirtAddr addr, size_t num)
	{
		for(size_t i = 0; i < NumAddressSpaces; i++)
		{
			if(addr >= AddressSpaces[i][0] && addr + ofsPages(num) <= AddressSpaces[i][1])
				return (AddressSpaceType) i;
		}

		abort("address {p} not in any address space", addr);
		return (AddressSpaceType) 0;
	}


	void init(scheduler::Process* proc)
	{
		constexpr VirtAddr kernelBootstrapPages[NumAddressSpaces] = {
			addrs::KERN_VMM_STACK0_BOOTSTRAP,
			addrs::KERN_VMM_STACK1_BOOTSTRAP,
			addrs::KERN_VMM_STACK2_BOOTSTRAP,
		};

		auto kproc = scheduler::getKernelProcess();
		bool isFirst = (proc == kproc);

		for(size_t i = 0; i < NumAddressSpaces; i++)
		{
			auto s = &proc->addrspace.getVMMState((AddressSpaceType) i);

			if(isFirst)
			{
				// the kernel process needs its own special bootstrap page.
				mapAddress(kernelBootstrapPages[i], pmm::allocate(1), 1, PAGE_WRITE | PAGE_PRESENT, proc);
				s->init(extmmNames[i], kernelBootstrapPages[i].addr(), (kernelBootstrapPages[i] + ofsPages(1)).addr(), kproc);
			}
			else
			{
				// user processes don't need an explicit bootstrap, since they will use vmm::allocate to get their
				// memory, and clean it up accordingly on exit.
				s->init(extmmNames[i], 0, 0, kproc);
			}

			s->deallocate(AddressSpaces[i][0].addr(), (AddressSpaces[i][1] - AddressSpaces[i][0]) / PAGE_SIZE);
		}

		// unmap the null page.
		if(isFirst) unmapAddress(VirtAddr::zero(), 1);
		else        setupAddrSpace(proc);
	}

	void destroy(scheduler::Process* proc)
	{
		assert(proc && proc->processId != 0);

		for(size_t i = 0; i < NumAddressSpaces; i++)
			proc->addrspace.getVMMState((AddressSpaceType) i).destroy();

		// note: we don't need to un-setupAddrSpace, because that doesn't
		// allocate any memory -- only address spaces.
	}



	// these are the address spaces that we can hand out:
	// USER_ADDRSPACE_BASE          ->      USER_ADDRSPACE_END
	// KERNEL_HEAP_BASE             ->      KERNEL_HEAP_END
	// KERNEL_VMM_ADDRSPACE_BASE    ->      KERNEL_VMM_ADDRSPACE_END

	VirtAddr allocateAddrSpace(size_t num, AddressSpaceType type, scheduler::Process* proc)
	{
		if(proc == 0) proc = scheduler::getCurrentProcess();
		assert(proc);

		if((size_t) type >= NumAddressSpaces)
			abort("allocateAddrSpace(): invalid address space '{}'!", type);


		auto ret = VirtAddr(proc->addrspace.getVMMState(type).allocate(num, [](addr_t, size_t) -> bool { return true; }));
		assert(ret.isPageAligned());

		return ret;
	}

	void deallocateAddrSpace(VirtAddr addr, size_t num, scheduler::Process* proc)
	{
		assert(addr.isPageAligned());

		if(proc == 0) proc = scheduler::getCurrentProcess();
		assert(proc);

		return proc->addrspace.getVMMState(getAddrSpace(addr, num)).deallocate(addr.addr(), num);
	}





	VirtAddr allocateSpecific(VirtAddr addr, size_t num, scheduler::Process* proc)
	{
		assert(addr.isPageAligned());

		if(proc == 0) proc = scheduler::getCurrentProcess();
		assert(proc);

		auto virt = VirtAddr(proc->addrspace.getVMMState(getAddrSpace(addr, num)).allocateSpecific(addr.addr(), num));
		if(virt.nonZero())  proc->addrspace.lock()->addRegion(virt, num);
		else                warn("vmm", "failed to allocate specific address ({p}, {})", addr, num);

		return virt;
	}

	VirtAddr allocate(size_t num, AddressSpaceType type, uint64_t flags, scheduler::Process* proc)
	{
		if(proc == 0) proc = scheduler::getCurrentProcess();
		assert(proc);

		auto virt = allocateAddrSpace(num, type, proc);
		if(virt.isZero()) { abort("vmm::allocate(): out of addrspace!"); }

		// don't have the write flag.
		flags &= ~PAGE_WRITE;

		mapZeroedCOW(virt, num, flags, proc);
		proc->addrspace.lock()->addRegion(virt, num);

		return virt;
	}

	VirtAddr allocateEager(size_t num, AddressSpaceType type, uint64_t flags, scheduler::Process* proc)
	{
		if(proc == 0) proc = scheduler::getCurrentProcess();
		assert(proc);

		auto virt = allocateAddrSpace(num, type, proc);
		if(virt.isZero()) { abort("vmm::allocate(): out of addrspace!"); return VirtAddr::zero(); }

		auto phys = pmm::allocate(num);
		if(phys.isZero()) { abort("vmm::allocate(): no physical pages!"); return VirtAddr::zero(); }

		mapAddress(virt, phys, num, flags | PAGE_PRESENT, proc);
		proc->addrspace.lock()->addRegion(virt, phys, num);

		return virt;
	}

	void deallocate(VirtAddr addr, size_t num, scheduler::Process* proc)
	{
		assert(addr.isPageAligned());

		if(proc == 0) proc = scheduler::getCurrentProcess();
		assert(proc);

		unmapAddress(addr, num, /* ignoreIfNotMapped: */ false, proc);
		deallocateAddrSpace(addr, num, proc);

		proc->addrspace.lock()->freeRegion(addr, num, /* freePhys: */ true);
	}
}
}






















