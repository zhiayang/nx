// page_fault.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"


namespace nx {
namespace vmm
{
	constexpr bool LOG_ALL_FAULTS = false;


	bool handlePageFault(uint64_t cr2, uint64_t errorCode, uint64_t rip)
	{
		// The error code gives us details of what happened.
		uint8_t notPresent  = !(errorCode & 0x1);   // page was not present
		uint8_t write       = errorCode & 0x2;      // fault happened when writing
		uint8_t reserved    = errorCode & 0x8;      // something bad happened

		if(reserved)
			return false;


		auto aligned_cr2 = VirtAddr(PAGE_ALIGN(cr2));

		// this will get the flags for the current process's address space, so we
		// don't need to pass it explicitly.
		auto flags = vmm::getPageFlags(aligned_cr2);


		/*
			make sure this is something that's a legitimate situation.

			flags legend:

			PRESENT & COW   -- COW, or zeroed-COW
			COW             -- lazy alloc
		*/
		if(flags & PAGE_COPY_ON_WRITE)
		{
			auto pid = scheduler::getCurrentProcess()->processId;
			auto tid = scheduler::getCurrentThread()->threadId;

			// ok, we should be sane here. we enable interrupts here, for a number of reasons:
			// 1. this handler might need to do complex work, so we don't want to hold up the
			//    entire CPU while that work is happening.
			// 2. we need to hold some locks (pmm, vmm), so to prevent deadlocking we need
			//    to be preemptible.

			// to make sure we can acquire spinlocks, tell the kernel that we're no longer in
			// an interrupt context. when we exit, the exception handler wrapper in asm will
			// also exit the intr_context so, we need to "enter" it again so the value never
			// becomes negative (on exit from this function)

			// even if we pagefault while holding a lock (externally), it should be fine because
			// we already hold that lock; so even if we get pre-empted here by another thread,
			// else should be able to go in and mess things up. so it should be safe to force-enable
			// interrupts with sti/cli instead of using interrupts::enable/disable.
			struct _ {
				_() { platform::leave_interrupt_context(); asm volatile ("sti"); }
				~_() { asm volatile ("cli"); platform::enter_interrupt_context(); }
			} __;

			// then, get a new one. note: we don't need to track this in the address space, because the
			// virtual address will have already been tracked, and we'll automatically free the physical
			// page when the process dies.

			// if the page wasn't present, then... wtf??
			if(notPresent || !(flags & PAGE_PRESENT))
			{
				// now we need to lookup the faulting address in the process' address space.
				auto proc = scheduler::getCurrentProcess();
				auto pair = proc->addrspace.lock()->lookupVirtualMapping(aligned_cr2);

				if(pair.first.present())
				{
					bool needsClearing = false;
					auto ph = pair.second;
					if(!ph.present())
					{
						// make a new one, then map it.
						auto p = pmm::allocate(1);
						proc->addrspace.lock()->addPhysicalMapping(aligned_cr2, p);

						ph = opt::some(p);
						needsClearing = true;
					}

					if(LOG_ALL_FAULTS)
						dbg("pf", "pid {} / tid {}: #PF (cr2={p}, ip={p}) -> phys {p}", pid, tid, cr2, rip, ph.get());

					vmm::mapAddressOverwrite(aligned_cr2, ph.get(), 1, (flags & ~PAGE_COPY_ON_WRITE) | PAGE_PRESENT, proc);

					if(needsClearing)
						memset(aligned_cr2.ptr(), 0, PAGE_SIZE);

					return true;
				}
				else
				{
					error("pf", "access of non-present CoW page? #PF (pid={}, tid={}, cr2={p}, ip={p})",
						pid, tid, cr2, rip);
					return false;
				}
			}
			else
			{
				// if the page was present but we faulted when reading, then... ?!
				if(!write)
				{
					error("pf", "read operation from CoW page caused #PF (pid={}, tid={}, cr2={p}, ip={p})",
						pid, tid, cr2, rip);
					return false;
				}

				auto phys = pmm::allocate(1);
				vmm::mapAddressOverwrite(aligned_cr2, phys, 1, (flags & ~PAGE_COPY_ON_WRITE) | PAGE_WRITE);
				{
					auto proc = scheduler::getCurrentProcess();
					proc->addrspace.lock()->addPhysicalMapping(aligned_cr2, phys);
				}

				// get the old physical address:
				auto old_phys = vmm::getPhysAddr(aligned_cr2);

				// if it was not the zero page, then memcopy,
				// else we're just wasting cycles copying zeroes so don't.
				if(old_phys != pmm::getZeroPage())
				{
					// ok, the dumb part is here we need to map a scratch page...
					auto scratch = vmm::allocateAddrSpace(1, AddressSpaceType::User);
					vmm::mapAddress(scratch, old_phys, 1, PAGE_NX | PAGE_USER | PAGE_PRESENT);

					// right, now we can copy:
					memmove(aligned_cr2.ptr(), scratch.ptr(), PAGE_SIZE);

					// ok, unmap (of course, don't free the old physical page!)
					vmm::unmapAddress(scratch, 1);

					// and free.
					vmm::deallocateAddrSpace(scratch, 1);
				}
				else
				{
					memset(aligned_cr2.ptr(), 0, PAGE_SIZE);
				}

				if(LOG_ALL_FAULTS)
					dbg("pf", "pid {} / tid {}: #PF (cr2={p}, ip={p}) -> {}", pid, tid, cr2, rip, phys);
			}

			return true;
		}
		else
		{
			// u dun goofed
			error("pf", "non-CoW page fault: tid {}, cr2={p}", scheduler::getCurrentThread()->threadId, cr2);
			return false;
		}
	}
}
}






