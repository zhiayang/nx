// vmm.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "bfx.h"
#include "string.h"

namespace bfx {
namespace vmm
{
	constexpr size_t indexPML4(uint64_t addr)       { return ((((uint64_t) addr) >> 39) & 0x1FF); }
	constexpr size_t indexPDPT(uint64_t addr)       { return ((((uint64_t) addr) >> 30) & 0x1FF); }
	constexpr size_t indexPageDir(uint64_t addr)    { return ((((uint64_t) addr) >> 21) & 0x1FF); }
	constexpr size_t indexPageTable(uint64_t addr)  { return ((((uint64_t) addr) >> 12) & 0x1FF); }

	constexpr uint64_t PAGE_PRESENT     = 0x1;
	constexpr uint64_t PAGE_WRITE       = 0x2;
	constexpr uint64_t PAGE_USER        = 0x4;
	constexpr uint64_t PAGE_NX          = 0x8000'0000'0000'0000;

	constexpr uint64_t HIGHER_HALF_BASE = 0xFFFF'0000'0000'0000ULL;

	constexpr size_t PDPT_SIZE = 0x80'0000'0000ULL;
	constexpr size_t PDIR_SIZE = 0x4000'0000ULL;
	constexpr size_t PTAB_SIZE = 0x20'0000ULL;

	static uint64_t end(uint64_t base, size_t num)  { return base + (num * PAGE_SIZE); }


	// well, since i can't get this to work as a constexpr function...
	template <size_t p4idx = 510>
	struct RecursiveAddr
	{
		static constexpr uint64_t pml4 = HIGHER_HALF_BASE + (p4idx * PDPT_SIZE) + (p4idx * PDIR_SIZE)
														  + (p4idx * PTAB_SIZE) + (p4idx * PAGE_SIZE);

		static constexpr uint64_t pdpt = HIGHER_HALF_BASE + (p4idx * PDPT_SIZE) + (p4idx * PDIR_SIZE)
														  + (p4idx * PTAB_SIZE);

		static constexpr uint64_t pdir = HIGHER_HALF_BASE + (p4idx * PDPT_SIZE) + (p4idx * PDIR_SIZE);
		static constexpr uint64_t ptab = HIGHER_HALF_BASE + (p4idx * PDPT_SIZE);
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

	static void invalidate(uint64_t addr)
	{
		asm volatile ("invlpg (%0)" :: "r"(addr));
	}

	void bootstrap(uint64_t physBase, uint64_t v, size_t maxPages)
	{
		// over here, we need to setup the recursive mapping.
		// in the kernel, we assume that the loader set it up for us
		// since we are the loader, we need to do that.
		{
			uint64_t cr3 = 0;
			asm volatile ("mov %%cr3, %0" : "=r"(cr3));

			// note: bootboot identity maps the lower 4g for us, so this is fine.
			auto pml4 = (pml_t*) cr3;
			pml4->entries[510] = ((uint64_t) pml4 | PAGE_PRESENT | PAGE_WRITE);
		}

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
		}

		// ok it should be set right now
		// hopefully we do not triple fault!!!!
		memset((void*) v, 0, PAGE_SIZE);
	}
}
}






















