// memory.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "efx.h"
#include "efi/system-table.h"
#include "efi/boot-services.h"

#include "string.h"

// for the vmm structs.
#include "kernel.h"

namespace efx
{
	void* efi_allocator::allocate(size_t sz)                { return efi::alloc(sz); }
	void efi_allocator::deallocate(void* ptr)               { return efi::free(ptr); }
}

namespace efx {
namespace memory
{
	static bool didInitArray = false;
	static efx::array<uint64_t> usedPages;
	void markPhyiscalPagesUsed(uint64_t addr, size_t num)
	{
		if(!didInitArray)
		{
			usedPages = efx::array<uint64_t>();
			didInitArray = true;
		}


		for(uint64_t x = addr, i = 0; i < num; x += 0x1000, i++)
			usedPages.append(x);
	}

	static uint64_t allocate_pagetab(uint64_t flags)
	{
		void* ret = 0;

		auto bs = efi::systable()->BootServices;
		auto stat = bs->AllocatePages(AllocateAnyPages, EfiLoaderData, 1, (uint64_t*) &ret);
		efi::abort_if_error(stat, "failed to allocate page!");

		memset(ret, 0, 0x1000);

		markPhyiscalPagesUsed((uint64_t) ret, 1);
		return (uint64_t) ret;
	}

	static nx::vmm::pml4t_t* pml4t_addr = 0;
	void setupCR3()
	{
		using namespace nx::vmm;

		// pml4:    256 tb
		// pdpt:    512 gb
		// pd:      1 gb
		// pt:      2 mb
		// pe:      4 kb

		// identity map the first 4 mb -- requiring 2 page-table entries.
		auto pml4 = (pml4t_t*) allocate_pagetab(0);
		auto pdpt = (pml4->entries[0] = (pdpt_t*) allocate_pagetab(PAGE_PRESENT | PAGE_WRITE));
		auto pagedir = (pdpt->entries[0] = (pagedir_t*) allocate_pagetab(PAGE_PRESENT | PAGE_WRITE));

		// we need 2 page tab;es.
		auto pt1 = (pagedir->entries[0] = (pagetable_t*) allocate_pagetab(PAGE_PRESENT | PAGE_WRITE));
		auto pt2 = (pagedir->entries[1] = (pagetable_t*) allocate_pagetab(PAGE_PRESENT | PAGE_WRITE));

		// identity map them.
		for(uint64_t i = 0; i < 512; i++) pt1->entries[i] = (i * 0x1000) | PAGE_PRESENT | PAGE_WRITE;
		for(uint64_t i = 0; i < 512; i++) pt2->entries[i] = ((i + 512) * 0x1000) | PAGE_PRESENT | PAGE_WRITE;

		// also, recursively map.
		// BUT! since we are using the top 2GB for our higher-half kernel, we do the recursive map in the second-last
		// slot (index 510) instead of the last one (511).
		pml4->entries[510] = (pdpt_t*) pml4;

		pml4t_addr = pml4;
	}


	void mapVirtual(uint64_t phys, uint64_t virt, size_t num)
	{
		using namespace nx::vmm;

		if(!pml4t_addr) efi::abort("mapVirtual(): must call setupCR3() first!");

		for(size_t i = 0; i < num; i++)
		{
			uint64_t p = phys + (0x1000 * i);
			uint64_t v = virt + (0x1000 * i);

			// right.
			auto p4idx = indexPML4(v);
			auto p3idx = indexPDPT(v);
			auto p2idx = indexPageDir(v);
			auto p1idx = indexPageTable(v);

			if(p4idx == 510) efi::abort("mapVirtual(): cannot map to 510th PML4 entry!");

			if(pml4t_addr->entries[p4idx] == 0)
				pml4t_addr->entries[p4idx] = (pdpt_t*) allocate_pagetab(PAGE_PRESENT | PAGE_WRITE);

			auto pdpt = pml4t_addr->entries[p4idx];


			if(pdpt->entries[p3idx] == 0)
				pdpt->entries[p3idx] = (pagedir_t*) allocate_pagetab(PAGE_PRESENT | PAGE_WRITE);

			auto pdir = pdpt->entries[p3idx];


			if(pdir->entries[p2idx] == 0)
				pdir->entries[p2idx] = (pagetable_t*) allocate_pagetab(PAGE_PRESENT | PAGE_WRITE);

			auto ptable = pdir->entries[p2idx];

			ptable->entries[p1idx] = p | PAGE_PRESENT | PAGE_WRITE;
		}
	}
}
}






















namespace efi
{
	#define BLKHEADER_MAGIC 0xDEADC0DEBEEFFACE

	// add some rudimentary stuff
	struct blkheader_t
	{
		uint64_t magic;
		size_t length;
	};

	void* alloc(size_t sz)
	{
		if(sz == 0) return nullptr;

		void* ret = 0;
		auto stat = efi::systable()->BootServices->AllocatePool(EfiLoaderData, sz, &ret);
		if(EFI_ERROR(stat) || ret == 0)
			efi::abort("failed to allocate memory! (requested %zu bytes) (status: %d)", sz, stat);

		memset(ret, 0, sz);
		return ret;
	}

	void free(void* ptr)
	{
		if(ptr) efi::systable()->BootServices->FreePool(ptr);
	}
}
