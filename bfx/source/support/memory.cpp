// memory.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "string.h"

#include "bfx.h"
#include "bootboot.h"
#include "../../kernel/include/misc/addrs.h"

namespace bfx
{
	namespace vmm
	{
		constexpr uint64_t PAGE_PRESENT     = 0x1;
		constexpr uint64_t PAGE_WRITE       = 0x2;
		constexpr uint64_t PAGE_USER        = 0x4;
		constexpr uint64_t PAGE_ALIGN       = ~0xFFF;

		constexpr size_t indexPML4(uintptr_t addr)       { return ((((uintptr_t) addr) >> 39) & 0x1FF); }
		constexpr size_t indexPDPT(uintptr_t addr)       { return ((((uintptr_t) addr) >> 30) & 0x1FF); }
		constexpr size_t indexPageDir(uintptr_t addr)    { return ((((uintptr_t) addr) >> 21) & 0x1FF); }
		constexpr size_t indexPageTable(uintptr_t addr)  { return ((((uintptr_t) addr) >> 12) & 0x1FF); }

		static uint64_t allocate_pagetab(uint64_t flags)
		{
			uint64_t ret = pmm::allocate(1);
			if(!ret) abort("failed to allocate page!");

			memset((void*) ret, 0, 0x1000);
			return ret | flags;
		}


		static pml_t* pml4t_addr = 0;

		uint64_t getPML4Address()
		{
			return (uint64_t) pml4t_addr;
		}

		constexpr uint64_t IDENTITY_MAP_ADDR_MAX = 0x0100'0000;
		uint64_t getIdentityMaxAddr()
		{
			return IDENTITY_MAP_ADDR_MAX;
		}

		void setupCR3(BOOTBOOT* bbinfo)
		{
			// pml4:    256 tb
			// pdpt:    512 gb
			// pd:      1 gb
			// pt:      2 mb
			// pe:      4 kb

			auto pml4 = (pml_t*) allocate_pagetab(0);
			auto pdpt = (pml_t*) ((pml4->entries[0] = allocate_pagetab(PAGE_PRESENT | PAGE_WRITE)) & PAGE_ALIGN);
			auto pagedir = (pml_t*) ((pdpt->entries[0] = allocate_pagetab(PAGE_PRESENT | PAGE_WRITE)) & PAGE_ALIGN);

			constexpr size_t numPageTables = IDENTITY_MAP_ADDR_MAX / 0x200000;

			pml_t* pts[numPageTables];
			for(int i = 0; i < numPageTables; i++)
				pts[i] = (pml_t*) ((pagedir->entries[i] = allocate_pagetab(PAGE_PRESENT | PAGE_WRITE)) & PAGE_ALIGN);

			// identity map them.
			for(int k = 0; k < numPageTables; k++)
			{
				for(uint64_t i = 0; i < 512; i++)
					pts[k]->entries[i] = ((i + 512*k) * 0x1000) | PAGE_PRESENT | PAGE_WRITE;
			}

			// also, recursively map.
			// BUT! since we are using the top 2GB for our higher-half kernel, we do the recursive map in the second-last
			// slot (index 510) instead of the last one (511).
			pml4->entries[510] = ((uint64_t) pml4 | PAGE_PRESENT | PAGE_WRITE);

			pml4t_addr = pml4;

			// ok, map the framebuffer as well.
			if(auto fbaddr = (uint64_t) bbinfo->fb_ptr; fbaddr != 0)
			{
				// TODO: hack! map the framebuffer as user
				auto fbsz = (bbinfo->fb_scanline * bbinfo->fb_height);
				auto pgs = (fbsz + 0xFFF) / 0x1000;

				mapKernel(fbaddr, nx::addrs::KERNEL_FRAMEBUFFER, pgs, PAGE_USER);
				mmap::addEntry(fbaddr, pgs, (uint64_t) nx::MemoryType::Framebuffer);
			}

			println("setup kernel CR3: %p", pml4t_addr);
			mmap::addEntry((uint64_t) pml4t_addr, 1, (uint64_t) nx::MemoryType::LoadedKernel);
		}


		void mapKernel(uint64_t phys, uint64_t virt, size_t num, uint64_t flags)
		{
			if(!pml4t_addr) abort("mapVirtual(): must call setupCR3() first!");

			constexpr uint64_t DEFAULT_FLAGS = PAGE_USER | PAGE_WRITE | PAGE_PRESENT;
			for(size_t i = 0; i < num; i++)
			{
				uint64_t p = phys + (0x1000 * i);
				uint64_t v = virt + (0x1000 * i);

				// right.
				auto p4idx = indexPML4(v);
				auto p3idx = indexPDPT(v);
				auto p2idx = indexPageDir(v);
				auto p1idx = indexPageTable(v);

				if(p4idx == 510) abort("mapVirtual(): cannot map to 510th PML4 entry!");

				if(pml4t_addr->entries[p4idx] == 0)
					pml4t_addr->entries[p4idx] = allocate_pagetab(DEFAULT_FLAGS | flags);

				auto pdpt = (pml_t*) (pml4t_addr->entries[p4idx] & PAGE_ALIGN);

				if(pdpt->entries[p3idx] == 0)
					pdpt->entries[p3idx] = allocate_pagetab(DEFAULT_FLAGS | flags);

				auto pdir = (pml_t*) (pdpt->entries[p3idx] & PAGE_ALIGN);

				if(pdir->entries[p2idx] == 0)
					pdir->entries[p2idx] = allocate_pagetab(DEFAULT_FLAGS | flags);

				auto ptable = (pml_t*) (pdir->entries[p2idx] & PAGE_ALIGN);

				ptable->entries[p1idx] = p | PAGE_PRESENT | PAGE_WRITE | flags;
			}
		}
	}
}






