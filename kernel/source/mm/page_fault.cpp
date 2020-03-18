// page_fault.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

extern "C" void nx_x64_enter_intr_context();
extern "C" void nx_x64_exit_intr_context();

namespace nx {
namespace vmm
{
	bool handlePageFault(uint64_t cr2, uint64_t errorCode, uint64_t rip)
	{
		// The error code gives us details of what happened.
		uint8_t notPresent  = !(errorCode & 0x1);   // page was not present
		uint8_t write       = errorCode & 0x2;      // fault happened when writing
		uint8_t reserved    = errorCode & 0x8;      // something bad happened

		if(reserved)
			return false;


		auto aligned_cr2 = vmm::PAGE_ALIGN(cr2);

		// this will get the flags for the current process's address space, so we
		// don't need to pass it explicitly.
		auto flags = vmm::getPageFlags(aligned_cr2);


		// make sure this is something that's a legitimate situation.
		if(flags & PAGE_COPY_ON_WRITE)
		{
			auto pid = scheduler::getCurrentProcess()->processId;
			auto tid = scheduler::getCurrentThread()->threadId;

			// if the page wasn't present, then... wtf??
			if(notPresent || !(flags & PAGE_PRESENT))
			{
				error("pf", "access of non-present CoW page? #PF (pid=%lu, tid=%lu, cr2=%p, ip=%p)",
					pid, tid, cr2, rip);
				return false;
			}

			// if we weren't writing, then... wtf??
			if(!write)
			{
				error("pf", "read operation from CoW page caused #PF (pid=%lu, tid=%lu, cr2=%p, ip=%p)",
					pid, tid, cr2, rip);
				return false;
			}

			// ok, we should be sane here. we enable interrupts here, for a number of reasons:
			// 1. this handler might need to do complex work, so we don't want to hold up the
			//    entire CPU while that work is happening.
			// 2. we need to hold some locks (pmm, vmm), so to prevent deadlocking we need
			//    to be preemptible.

			// to make sure we can acquire spinlocks, tell the kernel that we're no longer in
			// an interrupt context:
			nx_x64_exit_intr_context();
			asm volatile ("sti");

			// get the old physical address:
			auto old_phys = vmm::getPhysAddr(aligned_cr2);

			// then, get a new one. note: we don't need to track this in the address space, because the
			// virtual address will have already been tracked, and we'll automatically free the physical
			// page when the process dies.

			warn("pf", "allocating page");
			auto phys = pmm::allocate(1);
			vmm::mapAddressOverwrite(aligned_cr2, phys, 1, (flags & ~PAGE_COPY_ON_WRITE) | PAGE_WRITE);

			// if it was not the zero page, then memcopy,
			// else we're just wasting cycles copying zeroes so don't.
			if(old_phys != pmm::getZeroPage())
			{
				// ok, the dumb part is here we need to map a scratch page...
				auto scratch = vmm::allocateAddrSpace(1, AddressSpaceType::User);
				vmm::mapAddress(scratch, old_phys, 1, PAGE_NX | PAGE_USER | PAGE_PRESENT);

				// right, now we can copy:
				memmove((void*) aligned_cr2, (void*) scratch, PAGE_SIZE);

				// ok, unmap (of course, don't free the old physical page!)
				vmm::unmapAddress(scratch, 1, /* freePhys: */ false);

				// and free.
				vmm::deallocateAddrSpace(scratch, 1);
			}


			log("pf", "pid %lu / tid %lu: #PF (cr2=%p, ip=%p) -> phys %p", pid, tid, cr2, rip, phys);


			// here's the thing: the exception handler wrapper in asm will also exit the intr_context;
			// so, we need to "enter" it again so the value never becomes negative.
			asm volatile ("cli");
			nx_x64_enter_intr_context();

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






