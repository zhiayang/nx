// virtmap.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace vmm
{
	void mapAddress(addr_t virt, addr_t phys, size_t num, uint64_t flags)
	{
		virt &= PAGE_ALIGN;
		phys &= PAGE_ALIGN;

		for(size_t i = 0; i < num; i++)
		{
			addr_t v = virt + (i * PAGE_SIZE);
			addr_t p = phys + (i * PAGE_SIZE);

			// right.
			auto p4idx = indexPML4(v);
			auto p3idx = indexPDPT(v);
			auto p2idx = indexPageDir(v);
			auto p1idx = indexPageTable(v);

			if(p4idx == 510) abort("cannot map to PML4T at index 510!");

			auto pml4 = (pml_t*) RecursiveAddrs[0];
			if(!(pml4->entries[p4idx] & PAGE_PRESENT))
			{
				pml4->entries[p4idx] = pmm::allocate(1) | PAGE_PRESENT | PAGE_WRITE;
				invalidate(pml4->entries[p4idx]);
			}

			auto pdpt = (pml_t*) (RecursiveAddrs[1] + 0x1000ULL * p4idx);
			if(!(pdpt->entries[p3idx] & PAGE_PRESENT))
			{
				pdpt->entries[p3idx] = pmm::allocate(1) | PAGE_PRESENT | PAGE_WRITE;
				invalidate(pdpt->entries[p3idx]);
			}

			auto pdir = (pml_t*) (RecursiveAddrs[2] + 0x20'0000ULL * p4idx + 0x1000ULL * p3idx);
			if(!(pdir->entries[p2idx] & PAGE_PRESENT))
			{
				pdir->entries[p2idx] = pmm::allocate(1) | PAGE_PRESENT | PAGE_WRITE;
				invalidate(pdir->entries[p2idx]);
			}

			auto ptab = (pml_t*) (RecursiveAddrs[3] + 0x4000'0000ULL * p4idx + 0x20'0000ULL * p3idx + 0x1000ULL * p2idx);

			ptab->entries[p1idx] = p | PAGE_PRESENT | PAGE_WRITE;
			invalidate(ptab->entries[p1idx]);
		}
	}


	void unmapAddress(addr_t virt, size_t num)
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

			auto pml4 = (pml_t*) RecursiveAddrs[0];
			if(!(pml4->entries[p4idx] & PAGE_PRESENT))
				abort("%p was not mapped! (pdpt not present)", virt);

			auto pdpt = (pml_t*) (RecursiveAddrs[1] + 0x1000ULL * p4idx);
			if(!(pdpt->entries[p3idx] & PAGE_PRESENT))
				abort("%p was not mapped! (pdir not present)", virt);

			auto pdir = (pml_t*) (RecursiveAddrs[2] + 0x20'0000ULL * p4idx + 0x1000ULL * p3idx);
			if(!(pdir->entries[p2idx] & PAGE_PRESENT))
				abort("%p was not mapped! (ptab not present)", virt);

			auto ptab = (pml_t*) (RecursiveAddrs[3] + 0x4000'0000ULL * p4idx + 0x20'0000ULL * p3idx + 0x1000ULL * p2idx);
			if(!(ptab->entries[p1idx] & PAGE_PRESENT))
				abort("%p was not mapped! (page not present)", virt);

			ptab->entries[p1idx] = 0;
			invalidate(ptab->entries[p1idx]);
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

		auto pml4 = (pml_t*) RecursiveAddrs[0];
		if(!(pml4->entries[p4idx] & PAGE_PRESENT))
			abort("%p was not mapped! (pdpt not present)", virt);

		auto pdpt = (pml_t*) (RecursiveAddrs[1] + 0x1000ULL * p4idx);
		if(!(pdpt->entries[p3idx] & PAGE_PRESENT))
			abort("%p was not mapped! (pdir not present)", virt);

		auto pdir = (pml_t*) (RecursiveAddrs[2] + 0x20'0000ULL * p4idx + 0x1000ULL * p3idx);
		if(!(pdir->entries[p2idx] & PAGE_PRESENT))
			abort("%p was not mapped! (ptab not present)", virt);

		auto ptab = (pml_t*) (RecursiveAddrs[3] + 0x4000'0000ULL * p4idx + 0x20'0000ULL * p3idx + 0x1000ULL * p2idx);
		if(!(ptab->entries[p1idx] & PAGE_PRESENT))
			abort("%p was not mapped! (page not present)", virt);

		return ptab->entries[p1idx] & PAGE_ALIGN;
	}




	void invalidate(addr_t addr)
	{
		asm volatile("invlpg (%0)" :: "r"(addr));
	}
}
}






















