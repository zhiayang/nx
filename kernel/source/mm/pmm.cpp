// pmm.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace pmm
{
	struct extent_t
	{
		uint64_t addr;
		uint64_t size;
	};


	// this is the number of pages that we'll use for our bootstrap of the memory manager.
	static constexpr size_t NumReservedPages = 8;
	static constexpr size_t extentsPerPage = (PAGE_SIZE / sizeof(extent_t));

	static extent_t* extentArray = (extent_t*) addrs::PMM_STACK_BASE;
	static size_t numExtents = 0;
	static size_t extentArrayPages = 0;

	static addr_t end(addr_t base, size_t num)  { return base + (num * PAGE_SIZE); }
	static addr_t end(extent_t* ext)            { return ext->addr + (ext->size * PAGE_SIZE); }


	static void extendExtentArray();
	static void bootstrap(BootInfo* bootinfo, addr_t base);

	void init(BootInfo* bootinfo)
	{
		// find a bootstrappable chunk of memory marked as usable.
		bool bootstrapped = false;
		{
			for(size_t i = 0; i < bootinfo->mmEntryCount; i++)
			{
				if(bootinfo->mmEntries[i].memoryType == MemoryType::Available && bootinfo->mmEntries[i].numPages >= NumReservedPages)
				{
					// ok. modify the map first to steal the memory:
					addr_t base = bootinfo->mmEntries[i].address;

					bootinfo->mmEntries[i].address += (NumReservedPages * 0x1000);
					bootinfo->mmEntries[i].numPages -= NumReservedPages;

					// bootstrap!
					bootstrap(bootinfo, base);
					bootstrapped = true;
					break;
				}
			}
		}

		if(!bootstrapped) abort("failed to bootstrap pmm!!");


		// ok, now loop through each memory entry for real.
		for(size_t i = 0; i < bootinfo->mmEntryCount; i++)
		{
			auto entry = &bootinfo->mmEntries[i];
			if(entry->memoryType == MemoryType::Available && entry->numPages >= NumReservedPages)
			{
				extent_t ext;
				ext.addr = entry->address;
				ext.size = entry->numPages;

				extentArray[numExtents] = ext;
				numExtents++;

				if(numExtents % extentsPerPage == 0)
				{
					// time to expand.
					extendExtentArray();
				}
			}
		}

		println("pmm initialised with %zu extents\n", numExtents);
	}


	addr_t allocate(size_t num, bool below4G)
	{
		if(num == 0) abort("pmm::allocate(): cannot allocate 0 pages!");

		for(size_t i = 0; i < numExtents; i++)
		{
			auto ext = &extentArray[i];
			if(ext->size >= num && (below4G ? end(ext) < 0xFFFF'FFFF : true))
			{
				// return this.
				// take from the bottom.

				addr_t ret = ext->addr;
				ext->addr += (num * PAGE_SIZE);
				ext->size -= num;

				return ret;
			}
		}

		abort("pmm::allocate(): failed to allocate page!");
	}





















	static void extendExtentArray()
	{
		auto virt = addrs::PMM_STACK_BASE + (extentArrayPages * PAGE_SIZE);
		auto phys = allocate(1);

		vmm::mapAddress(virt, phys, 1, vmm::PAGE_PRESENT | vmm::PAGE_WRITE);
	}



	// we will get NumReservedPages of memory, starting at 'base'.
	static void bootstrap(BootInfo* bootinfo, addr_t base)
	{
		// we will do a very manual allocation of the first page.
		// in the worst case scenario we will need 4 pages to map one virtual page.

		// map exactly ONE page for the bootstrap.
		{
			size_t bootstrapUsed = 0;

			using namespace vmm;

			addr_t v = addrs::PMM_STACK_BASE;

			// right.
			auto p4idx = indexPML4(v);
			auto p3idx = indexPDPT(v);
			auto p2idx = indexPageDir(v);
			auto p1idx = indexPageTable(v);

			auto pml4 = (pml_t*) RecursiveAddrs[0];
			if(!(pml4->entries[p4idx] & PAGE_PRESENT))
			{
				pml4->entries[p4idx] = end(base, bootstrapUsed++) | PAGE_PRESENT | PAGE_WRITE;
				invalidate(pml4->entries[p4idx]);
			}

			auto pdpt = (pml_t*) (RecursiveAddrs[1] + 0x1000ULL * p4idx);
			if(!(pdpt->entries[p3idx] & PAGE_PRESENT))
			{
				pdpt->entries[p3idx] = end(base, bootstrapUsed++) | PAGE_PRESENT | PAGE_WRITE;
				invalidate(pdpt->entries[p3idx]);
			}

			auto pdir = (pml_t*) (RecursiveAddrs[2] + 0x20'0000ULL * p4idx + 0x1000ULL * p3idx);
			if(!(pdir->entries[p2idx] & PAGE_PRESENT))
			{
				pdir->entries[p2idx] = end(base, bootstrapUsed++) | PAGE_PRESENT | PAGE_WRITE;
				invalidate(pdir->entries[p2idx]);
			}

			auto ptab = (pml_t*) (RecursiveAddrs[3] + 0x4000'0000ULL * p4idx + 0x20'0000ULL * p3idx + 0x1000ULL * p2idx);

			ptab->entries[p1idx] = end(base, bootstrapUsed++) | PAGE_PRESENT | PAGE_WRITE;
			invalidate(ptab->entries[p1idx]);
		}

		// ok it should be set right now
		// hopefully we do not triple fault!!!!
		memset((void*) addrs::PMM_STACK_BASE, 0, 0x1000);
		extentArrayPages = 1;
	}
}
}



































