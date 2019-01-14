// pmm.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace pmm
{
	// this is the number of pages that we'll use for our bootstrap of the memory manager.
	static constexpr size_t NumReservedPages = 8;

	static void bootstrap(BootInfo* bootinfo, addr_t base);
	static addr_t end(addr_t base, size_t num)  { return base + (num * PAGE_SIZE); }

	static extmm::State extmmState;

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

		// now that we have bootstrapped one starting page for ourselves, we can init the ext mm.
		memset(&extmmState, 0, sizeof(extmm::State));
		extmm::init(&extmmState, "pmm", addrs::PMM_STACK_BASE, addrs::PMM_STACK_END);

		// ok, now loop through each memory entry for real.
		size_t totalMem = 0;
		for(size_t i = 0; i < bootinfo->mmEntryCount; i++)
		{
			auto entry = &bootinfo->mmEntries[i];
			if(entry->memoryType == MemoryType::Available)
			{
				deallocate(entry->address, entry->numPages);
				totalMem += entry->numPages * 0x1000;
			}
		}

		char buf[128] = { 0 };
		auto s = krt::util::humanSizedBytes(&buf[0], totalMem);
		buf[s] = 0;

		log("pmm", "initialised with %zu extents, %s", extmmState.numExtents, buf);
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
	}


	addr_t allocate(size_t num, bool below4G)
	{
		// lol. we can't have capturing lambdas without some kind of std::function
		// and nobody knows how to implement that shit.
		return extmm::allocate(&extmmState, num, below4G ? [](addr_t a, size_t l) -> bool {
			return end(a, l) < 0xFFFF'FFFF;
		} : [](addr_t, size_t) -> bool { return true; });
	}

	void deallocate(addr_t addr, size_t num)
	{
		extmm::deallocate(&extmmState, addr, num);
	}

}
}



































