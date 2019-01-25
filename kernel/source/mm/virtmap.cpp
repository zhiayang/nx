// virtmap.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace vmm
{
	static pml_t* getPML4()
	{
		return (pml_t*) (RecursiveAddrs[0]);
	}

	static pml_t* getPDPT(size_t p4idx)
	{
		return (pml_t*) (RecursiveAddrs[1] + 0x1000ULL * p4idx);
	}

	static pml_t* getPDir(size_t p4idx, size_t p3idx)
	{
		return (pml_t*) (RecursiveAddrs[2] + 0x20'0000ULL * p4idx + 0x1000ULL * p3idx);
	}

	static pml_t* getPTab(size_t p4idx, size_t p3idx, size_t p2idx)
	{
		return (pml_t*) (RecursiveAddrs[3] + 0x4000'0000ULL * p4idx + 0x20'0000ULL * p3idx + 0x1000ULL * p2idx);
	}

	static void reloadCR3()
	{
		asm volatile ("mov %%cr3, %%rax; mov %%rax, %%cr3" ::: "%rax");
	}

	void invalidate(addr_t addr)
	{
		asm volatile("invlpg (%0)" :: "r"(addr));
	}




	void mapAddress(addr_t virt, addr_t phys, size_t num, uint64_t flags)
	{
		virt &= PAGE_ALIGN;
		phys &= PAGE_ALIGN;

		for(size_t x = 0; x < num; x++)
		{
			addr_t v = virt + (x * PAGE_SIZE);
			addr_t p = phys + (x * PAGE_SIZE);

			// right.
			auto p4idx = indexPML4(v);
			auto p3idx = indexPDPT(v);
			auto p2idx = indexPageDir(v);
			auto p1idx = indexPageTable(v);

			if(p4idx == 510) abort("cannot map to PML4T at index 510!");

			auto pml4 = getPML4();
			auto pdpt = getPDPT(p4idx);
			auto pdir = getPDir(p4idx, p3idx);
			auto ptab = getPTab(p4idx, p3idx, p2idx);

			// we don't actually need to invalidate any of these, because x64 does not cache non-present entries!
			// here for reference.

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

			if(!(pml4->entries[p4idx] & PAGE_PRESENT))
				pml4->entries[p4idx] = pmm::allocate(1) | PAGE_PRESENT;

			if(!(pdpt->entries[p3idx] & PAGE_PRESENT))
				pdpt->entries[p3idx] = pmm::allocate(1) | PAGE_PRESENT;

			if(!(pdir->entries[p2idx] & PAGE_PRESENT))
				pdir->entries[p2idx] = pmm::allocate(1) | PAGE_PRESENT;

			if(ptab->entries[p1idx] & PAGE_PRESENT)
				abort("virtual addr %p was already mapped (to phys %p)!", v, ptab->entries[p1idx] & PAGE_PRESENT);

			ptab->entries[p1idx] = p | PAGE_PRESENT | flags;
			invalidate((addr_t) v);
		}
	}


	void unmapAddress(addr_t virt, size_t num, bool freePhys)
	{
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

			auto pml4 = getPML4();
			auto pdpt = getPDPT(p4idx);
			auto pdir = getPDir(p4idx, p3idx);
			auto ptab = getPTab(p4idx, p3idx, p2idx);

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
	}

	addr_t getPhysAddr(addr_t virt)
	{
		virt &= PAGE_ALIGN;

		// right.
		auto p4idx = indexPML4(virt);
		auto p3idx = indexPDPT(virt);
		auto p2idx = indexPageDir(virt);
		auto p1idx = indexPageTable(virt);

		if(p4idx == 510) abort("cannot unmap PML4T at index 510!");

		auto pml4 = getPML4();
		auto pdpt = getPDPT(p4idx);
		auto pdir = getPDir(p4idx, p3idx);
		auto ptab = getPTab(p4idx, p3idx, p2idx);

		if(!(pml4->entries[p4idx] & PAGE_PRESENT))
			abort("%p was not mapped! (pdpt not present)", virt);

		if(!(pdpt->entries[p3idx] & PAGE_PRESENT))
			abort("%p was not mapped! (pdir not present)", virt);

		if(!(pdir->entries[p2idx] & PAGE_PRESENT))
			abort("%p was not mapped! (ptab not present)", virt);

		if(!(ptab->entries[p1idx] & PAGE_PRESENT))
			abort("%p was not mapped! (page not present)", virt);

		return ptab->entries[p1idx] & PAGE_ALIGN;
	}



}
}






















