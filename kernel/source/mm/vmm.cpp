// vmm.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace vmm
{
	void mapAddress(addr_t virt, addr_t phys, size_t num, uint64_t flags)
	{
		for(size_t i = 0; i < num; i++)
		{
			addr_t v = virt + (i * PAGE_SIZE);
			addr_t p = phys + (i * PAGE_SIZE);

			// right.
			auto p4idx = indexPML4(v);
			auto p3idx = indexPDPT(v);
			auto p2idx = indexPageDir(v);
			auto p1idx = indexPageTable(v);

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

	void invalidate(addr_t addr)
	{
		asm volatile("invlpg (%0)" :: "r"(addr));
	}
}
}






















