// virtmap.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "devices/serial.h"

namespace nx {
namespace vmm
{
	constexpr addr_t HIGHER_HALF_BASE = 0xFFFF'0000'0000'0000ULL;

	constexpr size_t PDPT_SIZE = 0x80'0000'0000ULL;
	constexpr size_t PDIR_SIZE = 0x4000'0000ULL;
	constexpr size_t PTAB_SIZE = 0x20'0000ULL;


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
		asm volatile ("mov %%cr3, %%rax; mov %%rax, %%cr3" ::: "%rax");
	}

	void invalidate(addr_t addr)
	{
		asm volatile("invlpg (%0)" :: "r"(addr));
	}



	// sets up the initial address space for user processes.
	void setupAddrSpace(scheduler::Process* proc)
	{
		// make sure we allocated it.
		assert(proc->cr3);

		// ok, give me some temp space.
		auto virt = allocateAddrSpace(1, AddressSpace::User);
		mapAddress(virt, proc->cr3, 1, PAGE_PRESENT);

		auto pml4 = (pml_t*) virt;

		// setup recursive paging
		pml4->entries[510] = proc->cr3;

		// setup kernel maps.
		// note: we should always be calling this while in an address space that has already been setup
		// so getting pdpt 511 should not fault us.
		pml4->entries[511] = (addr_t) getPDPT(511);

		// ok i think that's it.
		unmapAddress(virt, 1, /* freePhys: */ false);
		deallocateAddrSpace(virt, 1);
	}






	void mapAddress(addr_t virt, addr_t phys, size_t num, uint64_t flags, scheduler::Process* proc)
	{
		if(proc == 0) proc = scheduler::getCurrentProcess();
		assert(proc);

		virt &= PAGE_ALIGN;
		phys &= PAGE_ALIGN;

		bool isOtherProc = (proc != scheduler::getCurrentProcess());
		if(isOtherProc)
		{
			getPML4()->entries[509] = proc->cr3 | PAGE_PRESENT;

			// we need to temp-map the 509th entry of the target pml4 to itself as well
			// to achieve that, get a temp thing to modify it.

			serial::debugprintf("509: %p, %p | %p\n", getPML4()->entries[509], getPML4(), getPML4<509>());

			println("\nstate: %p", &scheduler::getCurrentProcess()->vmmStates[0]);
			for(size_t i = 0; i < scheduler::getCurrentProcess()->vmmStates[0].numExtents; i++)
			{
				auto ext = scheduler::getCurrentProcess()->vmmStates[0].extents[i];
				println("%p - %p", ext.addr, ext.addr + (0x1000 * ext.size));
			}

			auto tmp = allocateAddrSpace(1, AddressSpace::User);
			serial::debugprintf("tmp addr is %p\n", tmp);

			println("\nstate: %p", &scheduler::getCurrentProcess()->vmmStates[0]);
			for(size_t i = 0; i < scheduler::getCurrentProcess()->vmmStates[0].numExtents; i++)
			{
				auto ext = scheduler::getCurrentProcess()->vmmStates[0].extents[i];
				println("%p - %p", ext.addr, ext.addr + (0x1000 * ext.size));
			}

			mapAddress(tmp, proc->cr3, 1, PAGE_PRESENT);

			((pml_t*) tmp)->entries[509] = proc->cr3 | PAGE_PRESENT;

			unmapAddress(tmp, 1, /* freePhys: */ false);

		}

		serial::debugprintf("%p -> %p / %zu (%p)\n", virt, phys, num, proc->cr3);

		for(size_t x = 0; x < num; x++)
		{
			addr_t v = virt + (x * PAGE_SIZE);
			addr_t p = phys + (x * PAGE_SIZE);

			// right.
			auto p4idx = indexPML4(v);
			auto p3idx = indexPDPT(v);
			auto p2idx = indexPageDir(v);
			auto p1idx = indexPageTable(v);

			if(p4idx == 510 || p4idx == 509) abort("cannot map to PML4T at index 510 or 509!");

			auto pml4 = (isOtherProc ? getPML4<509>() : getPML4());
			auto pdpt = (isOtherProc ? getPDPT<509>(p4idx) : getPDPT(p4idx));
			auto pdir = (isOtherProc ? getPDir<509>(p4idx, p3idx) : getPDir(p4idx, p3idx));
			auto ptab = (isOtherProc ? getPTab<509>(p4idx, p3idx, p2idx) : getPTab(p4idx, p3idx, p2idx));

			// serial::debugprint("three");
			if(!(pml4->entries[p4idx] & PAGE_PRESENT))
				pml4->entries[p4idx] = pmm::allocate(1) | PAGE_PRESENT;

			// serial::debugprint("four");
			if(!(pdpt->entries[p3idx] & PAGE_PRESENT))
				pdpt->entries[p3idx] = pmm::allocate(1) | PAGE_PRESENT;

			// serial::debugprint("five");
			if(!(pdir->entries[p2idx] & PAGE_PRESENT))
				pdir->entries[p2idx] = pmm::allocate(1) | PAGE_PRESENT;

			// serial::debugprint("six");
			if(ptab->entries[p1idx] & PAGE_PRESENT)
				abort("virtual addr %p was already mapped (to phys %p)!", v, ptab->entries[p1idx]);

			// serial::debugprint("seven");
			ptab->entries[p1idx] = p | flags | PAGE_PRESENT;
			invalidate((addr_t) v);
		}

		// undo
		if(isOtherProc) getPML4()->entries[509] = 0;
	}



	void unmapAddress(addr_t virt, size_t num, bool freePhys, scheduler::Process* proc)
	{
		if(proc == 0) proc = scheduler::getCurrentProcess();
		assert(proc);

		bool isOtherProc = (proc != scheduler::getCurrentProcess());
		if(isOtherProc) getPML4()->entries[509] = proc->cr3 | PAGE_PRESENT;

		virt &= PAGE_ALIGN;

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
				abort("%p was not mapped! (pdpt not present)", virt);

			if(!(pdpt->entries[p3idx] & PAGE_PRESENT))
				abort("%p was not mapped! (pdir not present)", virt);

			if(!(pdir->entries[p2idx] & PAGE_PRESENT))
				abort("%p was not mapped! (ptab not present)", virt);

			if(!(ptab->entries[p1idx] & PAGE_PRESENT))
				abort("%p was not mapped! (page not present)", virt);


			if(freePhys)
			{
				addr_t phys = ptab->entries[p1idx] & PAGE_ALIGN;
				assert(phys);

				pmm::deallocate(phys, 1);
			}

			ptab->entries[p1idx] = 0;
			invalidate(v);
		}

		if(isOtherProc) getPML4()->entries[509] = 0;
	}

	addr_t getPhysAddr(addr_t virt, scheduler::Process* proc)
	{
		if(proc == 0) proc = scheduler::getCurrentProcess();
		assert(proc);

		bool isOtherProc = (proc != scheduler::getCurrentProcess());
		if(isOtherProc) getPML4()->entries[509] = proc->cr3 | PAGE_PRESENT;


		virt &= PAGE_ALIGN;

		// right.
		auto p4idx = indexPML4(virt);
		auto p3idx = indexPDPT(virt);
		auto p2idx = indexPageDir(virt);
		auto p1idx = indexPageTable(virt);

		if(p4idx == 510) abort("cannot unmap PML4T at index 510!");

		auto pml4 = (isOtherProc ? getPML4<509>() : getPML4());
		auto pdpt = (isOtherProc ? getPDPT<509>(p4idx) : getPDPT(p4idx));
		auto pdir = (isOtherProc ? getPDir<509>(p4idx, p3idx) : getPDir(p4idx, p3idx));
		auto ptab = (isOtherProc ? getPTab<509>(p4idx, p3idx, p2idx) : getPTab(p4idx, p3idx, p2idx));

		if(!(pml4->entries[p4idx] & PAGE_PRESENT))
			abort("%p was not mapped! (pdpt not present)", virt);

		if(!(pdpt->entries[p3idx] & PAGE_PRESENT))
			abort("%p was not mapped! (pdir not present)", virt);

		if(!(pdir->entries[p2idx] & PAGE_PRESENT))
			abort("%p was not mapped! (ptab not present)", virt);

		if(!(ptab->entries[p1idx] & PAGE_PRESENT))
			abort("%p was not mapped! (page not present)", virt);

		auto phys = ptab->entries[p1idx] & PAGE_ALIGN;


		if(isOtherProc) getPML4()->entries[509] = 0;

		return phys;
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
				pml4->entries[p4idx] = end(physBase, bootstrapUsed++) | PAGE_PRESENT;

			if(!(pdpt->entries[p3idx] & PAGE_PRESENT))
				pdpt->entries[p3idx] = end(physBase, bootstrapUsed++) | PAGE_PRESENT;

			if(!(pdir->entries[p2idx] & PAGE_PRESENT))
				pdir->entries[p2idx] = end(physBase, bootstrapUsed++) | PAGE_PRESENT;

			ptab->entries[p1idx] = end(physBase, bootstrapUsed++) | PAGE_PRESENT;
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






















