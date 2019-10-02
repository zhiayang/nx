// pmm.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

#define NX_BOOTINFO_VERSION NX_SUPPORTED_BOOTINFO_VERSION
#include "bootinfo.h"



namespace nx {
namespace pmm
{
	// this is the number of pages that we'll use for our bootstrap of the memory manager.
	constexpr size_t NumReservedPages = 8;

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
					vmm::bootstrap(base, addrs::PMM_STACK_BASE, NumReservedPages);
					bootstrapped = true;
					break;
				}
			}
		}

		if(!bootstrapped) abort("failed to bootstrap pmm!!");

		// now that we have bootstrapped one starting page for ourselves, we can init the ext mm.
		extmm::init(&extmmState, "pmm", addrs::PMM_STACK_BASE, addrs::PMM_STACK_END);

		// ok, now loop through each memory entry for real.
		size_t totalMem = 0;
		for(size_t i = 0; i < bootinfo->mmEntryCount; i++)
		{
			auto entry = &bootinfo->mmEntries[i];
			if(entry->memoryType == MemoryType::Available)
			{
				// log("pmm", "extent: %p (%zu)", entry->address, entry->numPages);
				deallocate(entry->address, entry->numPages);
				totalMem += entry->numPages * 0x1000;
			}
		}

		log("pmm", "initialised with %zu extents, %zu bytes", extmmState.numExtents, totalMem);
	}

	void freeEarlyMemory(BootInfo* bootinfo, MemoryType type)
	{
		for(size_t i = 0; i < bootinfo->mmEntryCount; i++)
		{
			auto entry = &bootinfo->mmEntries[i];
			if(entry->memoryType == type)
			{
				deallocate(entry->address, entry->numPages);

				// for safety, make sure we don't touch this again (accidentally or otherwise)
				entry->memoryType = MemoryType::Reserved;
			}
		}
	}

	// this one frees the memory map, and the bootinfo itself.
	void freeAllEarlyMemory(BootInfo* bootinfo)
	{
		freeEarlyMemory(bootinfo, MemoryType::MemoryMap);

		auto addr = (addr_t) bootinfo;
		assert((addr & vmm::PAGE_ALIGN) == addr);

		// we assume the kernel boot info is one page long!!
		deallocate(addr, 1);

		log("pmm", "freed all bootloader memory");
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



































