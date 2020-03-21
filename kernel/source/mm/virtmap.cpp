// virtmap.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

#define NX_BOOTINFO_VERSION NX_MAX_BOOTINFO_VERSION
#include "bootinfo.h"

namespace nx {

// ewwwwww
extern uint32_t fbResX;
extern uint32_t fbResY;
extern uint32_t fbScan;

namespace vmm
{
	constexpr addr_t HIGHER_HALF_BASE = 0xFFFF'0000'0000'0000ULL;

	constexpr size_t PDPT_SIZE = 0x80'0000'0000ULL;
	constexpr size_t PDIR_SIZE = 0x4000'0000ULL;
	constexpr size_t PTAB_SIZE = 0x20'0000ULL;

	extern "C" uint8_t nx_user_kernel_stubs_begin;
	extern "C" uint8_t nx_user_kernel_stubs_end;


	static addr_t end(addr_t base, size_t num)  { return base + (num * PAGE_SIZE); }


	// well, since i can't get this to work as a constexpr function...
	template <size_t p4idx = 510>
	struct RecursiveAddr
	{
		static constexpr addr_t pml4 = HIGHER_HALF_BASE + (p4idx * PDPT_SIZE) + (p4idx * PDIR_SIZE) + (p4idx * PTAB_SIZE) + (p4idx * PAGE_SIZE);
		static constexpr addr_t pdpt = HIGHER_HALF_BASE + (p4idx * PDPT_SIZE) + (p4idx * PDIR_SIZE) + (p4idx * PTAB_SIZE);
		static constexpr addr_t pdir = HIGHER_HALF_BASE + (p4idx * PDPT_SIZE) + (p4idx * PDIR_SIZE);
		static constexpr addr_t ptab = HIGHER_HALF_BASE + (p4idx * PDPT_SIZE);
	};

	template <size_t recursiveIdx = 510>
	static pml_t* getPML4()
	{
		return (pml_t*) (RecursiveAddr<recursiveIdx>::pml4);
	}

	template <size_t recursiveIdx = 510>
	static pml_t* getPDPT(size_t p4idx)
	{
		return (pml_t*) (RecursiveAddr<recursiveIdx>::pdpt + (PAGE_SIZE * p4idx));
	}

	template <size_t recursiveIdx = 510>
	static pml_t* getPDir(size_t p4idx, size_t p3idx)
	{
		return (pml_t*) (RecursiveAddr<recursiveIdx>::pdir + (PTAB_SIZE * p4idx) + (PAGE_SIZE * p3idx));
	}

	template <size_t recursiveIdx = 510>
	static pml_t* getPTab(size_t p4idx, size_t p3idx, size_t p2idx)
	{
		return (pml_t*) (RecursiveAddr<recursiveIdx>::ptab + (PDIR_SIZE * p4idx) + (PTAB_SIZE * p3idx) + (PAGE_SIZE * p2idx));
	}

	void invalidateAll()
	{
		asm volatile ("mov %%cr3, %%rax; mov %%rax, %%cr3" ::: "rax");
	}

	void invalidate(addr_t addr)
	{
		asm volatile ("invlpg (%0)" :: "r"(addr));
	}

	void switchAddrSpace(addr_t cr3)
	{
		asm volatile ("mov %0, %%cr3" :: "a"(cr3) : "memory");
	}



	// sets up the initial address space for user processes.
	void setupAddrSpace(scheduler::Process* proc)
	{
		// make sure we allocated it.
		assert(proc->addrspace.cr3);

		// ok, give me some temp space.
		auto virt = allocateAddrSpace(1, AddressSpaceType::User);
		mapAddress(virt, proc->addrspace.cr3, 1, PAGE_PRESENT | PAGE_WRITE);

		auto pml4 = (pml_t*) virt;
		memset(pml4, 0, PAGE_SIZE);

		// setup recursive paging
		pml4->entries[510] = proc->addrspace.cr3 | PAGE_PRESENT | PAGE_WRITE;

		// setup kernel maps.
		// note: we should always be calling this while in an address space that has already been setup
		// so getting pdpt 511 should not fault us.
		pml4->entries[511] = getPhysAddr((addr_t) getPDPT(511)) | PAGE_WRITE | PAGE_PRESENT;


		// TODO: fuckin nasty man. bad hack!! we need to move this elsewhere
		{
			auto numPgs = (fbScan * fbResY * 4 + PAGE_SIZE - 1) / PAGE_SIZE;

			mapAddress(addrs::USER_FRAMEBUFFER, getPhysAddr(addrs::KERNEL_FRAMEBUFFER),
				numPgs, PAGE_USER | PAGE_WRITE | PAGE_PRESENT, proc);
		}

		// TODO: fuckin nasty hack part 2!!! we also need to move this elsewhere!!
		{
			auto stubs_begin = (uintptr_t) &nx_user_kernel_stubs_begin;
			auto stubs_end   = (uintptr_t) &nx_user_kernel_stubs_end;

			assert(isAligned(stubs_begin));
			auto numPgs = (stubs_end - stubs_begin + PAGE_SIZE - 1) / PAGE_SIZE;

			// we do not allocate memory -- just map the physical pages here.
			for(size_t i = 0; i < numPgs; i++)
			{
				auto kv = stubs_begin + (i * PAGE_SIZE);
				auto kp = getPhysAddr(kv);

				auto uv = addrs::USER_KERNEL_STUBS + (i * PAGE_SIZE);
				mapAddress(uv, kp, 1, PAGE_USER | PAGE_PRESENT, proc);
			}
		}


		// next, we need to map the lapic's base address as well, so we don't need to
		// change cr3 every time we want to send an EOI. it is always one page long.
		if(interrupts::hasLocalAPIC())
		{
			// note: we're not going to change the lapic base address,
			// and it should be the same for every CPU.
			auto lapicBase = scheduler::getCurrentCPU()->localApicAddr;
			if(allocateSpecific(lapicBase, 1, proc) != lapicBase)
				abort("failed to map lapic (at %p) to user process!", lapicBase);

			mapAddress(lapicBase, lapicBase, 1, PAGE_PRESENT | PAGE_WRITE, proc);
		}

		// ok i think that's it.
		unmapAddress(virt, 1);
		deallocateAddrSpace(virt, 1);
	}



	/*
		note: we only have one temporary mapping space -- pml4[509]. if we are preempted while
		performing an external-process mapping, we'll get our temporary mapping stomped!

		imagine: we map our page, then in the middle, we get preempted. the other guy comes in and also
		wants to map pages; they stomp over our original mapping, and once they're done they unmap it
		completely -- when we come back, everything is gone.

		so, we have a lock specifically to protect the 509 mapping -- hence its name.
	*/

	static nx::spinlock globalPml509Lock;
	struct TempMapper
	{
		TempMapper(scheduler::Process* p) : proc(p) { if(this->proc) { globalPml509Lock.lock(); this->map(); } }
		~TempMapper()                               { if(this->proc) { this->unmap(); globalPml509Lock.unlock(); } }

		TempMapper(const TempMapper&) = delete;
		TempMapper(const TempMapper&&) = delete;

		TempMapper& operator= (const TempMapper&) = delete;
		TempMapper& operator= (const TempMapper&&) = delete;


	private:
		scheduler::Process* proc;

		void map()
		{
			getPML4()->entries[509] = this->proc->addrspace.cr3 | PAGE_PRESENT | PAGE_WRITE;

			// we need to temp-map the 509th entry of the target pml4 to itself as well
			// to achieve that, get a temp thing to modify it.

			auto tmp = allocateAddrSpace(1, AddressSpaceType::User);
			mapAddress(tmp, this->proc->addrspace.cr3, 1, PAGE_PRESENT | PAGE_WRITE);

			((pml_t*) tmp)->entries[509] = this->proc->addrspace.cr3 | PAGE_PRESENT | PAGE_WRITE;

			unmapAddress(tmp, 1);
		}

		void unmap()
		{
			getPML4()->entries[509] = 0;

			auto tmp = allocateAddrSpace(1, AddressSpaceType::User);
			mapAddress(tmp, this->proc->addrspace.cr3, 1, PAGE_PRESENT | PAGE_WRITE);

			((pml_t*) tmp)->entries[509] = 0;

			unmapAddress(tmp, 1);
			invalidateAll();
		}
	};





	static addr_t alloc_phys_tracked(scheduler::Process* proc)
	{
		auto x = pmm::allocate(1);
		assert(x);

		// don't track the kernel!
		if(proc->processId > 0 || scheduler::getInitPhase() > scheduler::SchedulerInitPhase::SchedulerStarted)
		{
			LockedSection(&proc->addrSpaceLock, [proc, x]() {
				proc->addrspace.allocatedPhysPages.append(x);
			});
		}

		return x;
	};

	static void map_internal(addr_t virt, addr_t phys, uint64_t flags, scheduler::Process* proc, bool overwrite)
	{
		if(proc == 0) proc = scheduler::getCurrentProcess();
		assert(proc);

		assert(isAligned(virt));
		assert(isAligned(phys));

		bool isOtherProc = (proc != scheduler::getCurrentProcess());
		TempMapper tm(isOtherProc ? proc : 0);

		// note: the flags in the higher-level structures will override those at the lower level
		// ie. for a given page to be user-accessible, the pdpt, pdir, ptab, and the page itself must be marked PAGE_USER.
		// same applies for PAGE_WRITE (when CR0.WP=1)
		// also: for 0 <= p4idx <= 255, we set the user-bit on intermediate entries.
		// for 256 <= p4idx <= 511, we don't set it.
		constexpr addr_t DEFAULT_FLAGS_LOW  = PAGE_USER | PAGE_WRITE | PAGE_PRESENT;
		constexpr addr_t DEFAULT_FLAGS_HIGH = PAGE_WRITE | PAGE_PRESENT;

		// right.
		auto p4idx = indexPML4(virt);
		auto p3idx = indexPDPT(virt);
		auto p2idx = indexPageDir(virt);
		auto p1idx = indexPageTable(virt);

		if(p4idx == 510 || p4idx == 509) abort("cannot map to PML4T at index 510 or 509!");

		auto pml4 = (isOtherProc ? getPML4<509>() : getPML4());
		auto pdpt = (isOtherProc ? getPDPT<509>(p4idx) : getPDPT(p4idx));
		auto pdir = (isOtherProc ? getPDir<509>(p4idx, p3idx) : getPDir(p4idx, p3idx));
		auto ptab = (isOtherProc ? getPTab<509>(p4idx, p3idx, p2idx) : getPTab(p4idx, p3idx, p2idx));

		addr_t DEFAULT_FLAGS = (p4idx > 255 ? DEFAULT_FLAGS_HIGH : DEFAULT_FLAGS_LOW);
		if(p4idx > 255 && (flags & PAGE_USER))
			abort("cannot map high address space as user-readable!");

		if(!(pml4->entries[p4idx] & PAGE_PRESENT))
		{
			auto pp = alloc_phys_tracked(proc);
			assert(pp);

			pml4->entries[p4idx] = pp | DEFAULT_FLAGS;
			memset(pdpt, 0, PAGE_SIZE);

			if(pml4->entries[p4idx] & PAGE_NX)
				abort("somehow page %p has a NX pdpt! (p=%p)", virt, pp);
		}

		if(!(pdpt->entries[p3idx] & PAGE_PRESENT))
		{
			auto pp = alloc_phys_tracked(proc);
			assert(pp);

			pdpt->entries[p3idx] = pp | DEFAULT_FLAGS;
			memset(pdir, 0, PAGE_SIZE);

			if(pdpt->entries[p3idx] & PAGE_NX)
				abort("somehow page %p has a NX pdir! (p=%p)", virt, pp);
		}

		if(!(pdir->entries[p2idx] & PAGE_PRESENT))
		{
			auto pp = alloc_phys_tracked(proc);
			assert(pp);

			pdir->entries[p2idx] = pp | DEFAULT_FLAGS;
			memset(ptab, 0, PAGE_SIZE);

			if(pdir->entries[p2idx] & PAGE_NX)
				abort("somehow page %p has a NX ptab! (p=%p)", virt, pp);
		}

		if(!overwrite && (ptab->entries[p1idx] & PAGE_PRESENT))
			abort("virtual addr %p was already mapped (to phys %p)!", virt, PAGE_ALIGN(ptab->entries[p1idx]));

		// since this is the internal one, we don't add extra flags.
		ptab->entries[p1idx] = phys | flags;
		invalidate(virt);
	}



	void mapAddressOverwrite(addr_t virt, addr_t phys, size_t num, uint64_t flags, scheduler::Process* proc)
	{
		for(size_t i = 0; i < num; i++)
		{
			auto v = virt + (i * PAGE_SIZE);
			auto p = phys + (i * PAGE_SIZE);

			map_internal(v, p, flags | PAGE_PRESENT, proc, /* overwrite: */ true);
		}
	}

	void mapAddress(addr_t virt, addr_t phys, size_t num, uint64_t flags, scheduler::Process* proc)
	{
		for(size_t i = 0; i < num; i++)
		{
			auto v = virt + (i * PAGE_SIZE);
			auto p = phys + (i * PAGE_SIZE);

			map_internal(v, p, flags | PAGE_PRESENT, proc, /* overwrite: */ false);
		}
	}

	void mapCOW(addr_t virt, addr_t phys, size_t num, uint64_t flags, scheduler::Process* proc)
	{
		if(proc == 0) proc = scheduler::getCurrentProcess();
		assert(proc);

		assert(isAligned(virt));

		flags &= ~PAGE_PRESENT;
		log("vmm", "region %p - %p marked copy-on-write", virt, virt + (num * PAGE_SIZE));

		for(size_t i = 0; i < num; i++)
		{
			auto v = virt + (i * PAGE_SIZE);
			auto p = phys + (i * PAGE_SIZE);

			map_internal(v, p, flags | PAGE_PRESENT | PAGE_COPY_ON_WRITE, proc, /* overwrite: */ false);
		}
	}

	void mapLazy(addr_t virt, size_t num, uint64_t flags, scheduler::Process* proc)
	{
		if(proc == 0) proc = scheduler::getCurrentProcess();
		assert(proc);

		assert(isAligned(virt));

		// make sure we don't have write in the provided flags.
		assert((flags & PAGE_WRITE) == 0);
		flags &= ~PAGE_WRITE;

		log("vmm", "region %p - %p marked lazy", virt, virt + (num * PAGE_SIZE));

		for(size_t i = 0; i < num; i++)
		{
			auto v = virt + (i * PAGE_SIZE);

			// everybody gets the zero page. note: we mark the page present here, because reading
			// from a lazy page shouldn't actually do anything -- just return zeroes!
			map_internal(v, pmm::getZeroPage(), flags | PAGE_PRESENT | PAGE_COPY_ON_WRITE, proc, /* overwrite: */ false);
		}
	}


	uint64_t getPageFlags(addr_t virt, scheduler::Process* proc)
	{
		if(proc == 0) proc = scheduler::getCurrentProcess();
		assert(proc);

		assert(isAligned(virt));

		bool isOtherProc = (proc != scheduler::getCurrentProcess());
		TempMapper tm(isOtherProc ? proc : 0);

		// right.
		auto p4idx = indexPML4(virt);
		auto p3idx = indexPDPT(virt);
		auto p2idx = indexPageDir(virt);
		auto p1idx = indexPageTable(virt);

		auto pml4 = (isOtherProc ? getPML4<509>() : getPML4());
		auto pdpt = (isOtherProc ? getPDPT<509>(p4idx) : getPDPT(p4idx));
		auto pdir = (isOtherProc ? getPDir<509>(p4idx, p3idx) : getPDir(p4idx, p3idx));
		auto ptab = (isOtherProc ? getPTab<509>(p4idx, p3idx, p2idx) : getPTab(p4idx, p3idx, p2idx));

		if(!(pml4->entries[p4idx] & PAGE_PRESENT))
			return 0;

		if(!(pdpt->entries[p3idx] & PAGE_PRESENT))
			return 0;

		if(!(pdir->entries[p2idx] & PAGE_PRESENT))
			return 0;

		return ptab->entries[p1idx] & (PAGE_NX | 0xFFF);
	}



	void unmapAddress(addr_t virt, size_t num, bool ignore, scheduler::Process* proc)
	{
		if(proc == 0) proc = scheduler::getCurrentProcess();
		assert(proc);

		bool isOtherProc = (proc != scheduler::getCurrentProcess());
		TempMapper tm(isOtherProc ? proc : 0);

		assert(isAligned(virt));

		for(size_t i = 0; i < num; i++)
		{
			addr_t v = virt + (i * PAGE_SIZE);

			// right.
			auto p4idx = indexPML4(v);
			auto p3idx = indexPDPT(v);
			auto p2idx = indexPageDir(v);
			auto p1idx = indexPageTable(v);

			if(p4idx == 510) abort("cannot unmap PML4T at index 510!");

			auto pml4 = (isOtherProc ? getPML4<509>() : getPML4());
			auto pdpt = (isOtherProc ? getPDPT<509>(p4idx) : getPDPT(p4idx));
			auto pdir = (isOtherProc ? getPDir<509>(p4idx, p3idx) : getPDir(p4idx, p3idx));
			auto ptab = (isOtherProc ? getPTab<509>(p4idx, p3idx, p2idx) : getPTab(p4idx, p3idx, p2idx));

			if(!(pml4->entries[p4idx] & PAGE_PRESENT))
			{
				if(ignore) continue;
				abort("unmap: %p was not mapped! (pdpt not present)", virt);
			}

			if(!(pdpt->entries[p3idx] & PAGE_PRESENT))
			{
				if(ignore) continue;
				abort("unmap: %p was not mapped! (pdir not present)", virt);
			}

			if(!(pdir->entries[p2idx] & PAGE_PRESENT))
			{
				if(ignore) continue;
				abort("unmap: %p was not mapped! (ptab not present)", virt);
			}

			if(!(ptab->entries[p1idx] & PAGE_PRESENT))
			{
				if(ignore)
				{
					continue;
				}
				else if(ptab->entries[p1idx] & PAGE_COPY_ON_WRITE)
				{
					ptab->entries[p1idx] &= ~PAGE_COPY_ON_WRITE;
					continue;
				}

				abort("unmap: %p was not mapped! (page not present)", virt);
			}

			ptab->entries[p1idx] = 0;
			invalidate(v);
		}
	}


	addr_t getPhysAddr(addr_t virt, scheduler::Process* proc)
	{
		if(proc == 0) proc = scheduler::getCurrentProcess();
		assert(proc);

		bool isOtherProc = (proc != scheduler::getCurrentProcess());
		TempMapper tm(isOtherProc ? proc : 0);


		assert(isAligned(virt));

		// right.
		auto p4idx = indexPML4(virt);
		auto p3idx = indexPDPT(virt);
		auto p2idx = indexPageDir(virt);
		auto p1idx = indexPageTable(virt);

		auto pml4 = (isOtherProc ? getPML4<509>() : getPML4());
		auto pdpt = (isOtherProc ? getPDPT<509>(p4idx) : getPDPT(p4idx));
		auto pdir = (isOtherProc ? getPDir<509>(p4idx, p3idx) : getPDir(p4idx, p3idx));
		auto ptab = (isOtherProc ? getPTab<509>(p4idx, p3idx, p2idx) : getPTab(p4idx, p3idx, p2idx));

		if(!(pml4->entries[p4idx] & PAGE_PRESENT))
			abort("getPhys: %p was not mapped! (pdpt not present)", virt);

		if(!(pdpt->entries[p3idx] & PAGE_PRESENT))
			abort("getPhys: %p was not mapped! (pdir not present)", virt);

		if(!(pdir->entries[p2idx] & PAGE_PRESENT))
			abort("getPhys: %p was not mapped! (ptab not present)", virt);

		if(!(ptab->entries[p1idx] & PAGE_PRESENT))
			abort("getPhys: %p was not mapped! (page not present)", virt);

		return vmm::PAGE_ALIGN(ptab->entries[p1idx]);
	}


	bool isMapped(addr_t virt, size_t num, scheduler::Process* proc)
	{
		if(proc == 0) proc = scheduler::getCurrentProcess();
		assert(proc);

		bool isOtherProc = (proc != scheduler::getCurrentProcess());
		TempMapper tm(isOtherProc ? proc : 0);

		assert(isAligned(virt));

		for(size_t i = 0; i < num; i++)
		{
			addr_t v = virt + (i * PAGE_SIZE);

			// right.
			auto p4idx = indexPML4(v);
			auto p3idx = indexPDPT(v);
			auto p2idx = indexPageDir(v);
			auto p1idx = indexPageTable(v);

			auto pml4 = (isOtherProc ? getPML4<509>() : getPML4());
			auto pdpt = (isOtherProc ? getPDPT<509>(p4idx) : getPDPT(p4idx));
			auto pdir = (isOtherProc ? getPDir<509>(p4idx, p3idx) : getPDir(p4idx, p3idx));
			auto ptab = (isOtherProc ? getPTab<509>(p4idx, p3idx, p2idx) : getPTab(p4idx, p3idx, p2idx));

			if(!(pml4->entries[p4idx] & PAGE_PRESENT))  return false;
			if(!(pdpt->entries[p3idx] & PAGE_PRESENT))  return false;
			if(!(pdir->entries[p2idx] & PAGE_PRESENT))  return false;
			if(!(ptab->entries[p1idx] & PAGE_PRESENT))  return false;
		}

		return true;
	}


	void bootstrap(addr_t physBase, addr_t v, size_t maxPages)
	{
		// we will do a very manual allocation of the first page.
		// in the worst case scenario we will need 4 pages to map one virtual page.

		// map exactly ONE page for the bootstrap.
		{
			size_t bootstrapUsed = 0;

			// right.
			auto p4idx = indexPML4(v);
			auto p3idx = indexPDPT(v);
			auto p2idx = indexPageDir(v);
			auto p1idx = indexPageTable(v);

			auto pml4 = getPML4();
			auto pdpt = getPDPT(p4idx);
			auto pdir = getPDir(p4idx, p3idx);
			auto ptab = getPTab(p4idx, p3idx, p2idx);

			if(!(pml4->entries[p4idx] & PAGE_PRESENT))
			{
				pml4->entries[p4idx] = end(physBase, bootstrapUsed++) | PAGE_WRITE | PAGE_PRESENT;
				memset(pdpt, 0, PAGE_SIZE);
			}

			if(!(pdpt->entries[p3idx] & PAGE_PRESENT))
			{
				pdpt->entries[p3idx] = end(physBase, bootstrapUsed++) | PAGE_WRITE | PAGE_PRESENT;
				memset(pdir, 0, PAGE_SIZE);
			}

			if(!(pdir->entries[p2idx] & PAGE_PRESENT))
			{
				pdir->entries[p2idx] = end(physBase, bootstrapUsed++) | PAGE_WRITE | PAGE_PRESENT;
				memset(ptab, 0, PAGE_SIZE);
			}

			ptab->entries[p1idx] = end(physBase, bootstrapUsed++) | PAGE_WRITE | PAGE_PRESENT;
			invalidate(v);

			assert(bootstrapUsed <= maxPages);
		}

		// ok it should be set right now
		// hopefully we do not triple fault!!!!
		memset((void*) v, 0, 0x1000);
	}


	// here for reference:
	// we don't actually need to invalidate any of these, because x64 does not cache non-present entries!

	// invalidate((addr_t) pdpt);
	// for(int i = 0; i < 512; i++)
	// {
	// 	invalidate((addr_t) getPDir(p4idx, i));
	// 	for(int j = 0; j < 512; j++)
	// 	{
	// 		invalidate((addr_t) getPTab(p4idx, i, j));
	// 		for(int k = 0; k < 512; k++)
	// 		{
	// 			invalidate(indexToAddr(p4idx, i, j, k));
	// 		}
	// 	}
	// }

}
}






















