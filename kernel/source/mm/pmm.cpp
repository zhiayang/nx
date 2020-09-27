// pmm.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

#define NX_BOOTINFO_VERSION NX_MAX_BOOTINFO_VERSION
#include "bootinfo.h"



namespace nx {
namespace pmm
{
	// this is the number of pages that we'll use for our bootstrap of the memory manager.
	constexpr size_t NumReservedPages = 8;

	static addr_t end(addr_t base, size_t num)  { return base + (num * PAGE_SIZE); }
	static extmm::State<void, nx::spinlock> extmmState;
	static size_t totalPhysicalMemory = 0;
	static size_t numAllocatedBytes = 0;

	static PhysAddr zeroPageAddr { nullptr };
	PhysAddr getZeroPage()
	{
		return zeroPageAddr;
	}

	size_t getNumAllocatedBytes()   { return numAllocatedBytes; }
	size_t getTotalPhysicalMemory() { return totalPhysicalMemory; }

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
					vmm::bootstrap(base, addrs::PMM_STACK_BASE.addr(), NumReservedPages);
					bootstrapped = true;
					break;
				}
			}
		}

		if(!bootstrapped) abort("failed to bootstrap pmm!!");

		// now that we have bootstrapped one starting page for ourselves, we can init the ext mm.
		auto kproc = scheduler::getKernelProcess();
		assert(kproc);

		extmmState.init("pmm", addrs::PMM_STACK_BASE.addr(), addrs::PMM_STACK_END.addr(), kproc);

		// ok, now loop through each memory entry for real.
		for(size_t i = 0; i < bootinfo->mmEntryCount; i++)
		{
			auto entry = &bootinfo->mmEntries[i];
			if(entry->memoryType == MemoryType::Available && entry->address > 0)
			{
				extmmState.deallocate(entry->address, entry->numPages);
				totalPhysicalMemory += entry->numPages * 0x1000;
			}

			dbg("pmm", "extent: {p} - {p} ({} pages, {} bytes) - {#x}", entry->address, end(entry->address, entry->numPages),
				entry->numPages, entry->numPages * 0x1000, entry->memoryType);
		}

		// we should have > 1MB, if not UEFI wouldn't boot anyway.
		auto num = totalPhysicalMemory / (1024 * 1024); int unit = 0;
		char units[] = { 'M', 'G', 'T', 'P' };

		while(num >= 1024)
			num /= 1024, unit++;

		log("pmm", "initialised with {} extents, {} bytes ({} {}B)", extmmState.numExtents, totalPhysicalMemory,
			num, units[unit]);


		// find a single page to use as the zero-page. make sure it's within the identity mapped area, so
		// we can memset it without bothering with paging.
		auto zp = extmmState.allocate(1, [&bootinfo](addr_t x, size_t l) -> bool {
			return end(x, l) < bootinfo->maximumIdentityMap;
		});

		if(!zp) abort("pmm: failed to allocate zero page!");
		assert(zp < bootinfo->maximumIdentityMap);

		memset((void*) zp, 0, PAGE_SIZE);
		zeroPageAddr = PhysAddr(zp);

		// at this point, check for consistency
		if(!extmmState.checkConsistency())
			abort("inconsistent memory map from bootloader!");
	}

	void freeEarlyMemory(BootInfo* bootinfo, MemoryType type)
	{
		for(size_t i = 0; i < bootinfo->mmEntryCount; i++)
		{
			auto entry = &bootinfo->mmEntries[i];
			if(entry->memoryType == type)
			{
				assert(type != MemoryType::MemoryMap);

				// mark the memory is usable in the allocator, and additionally unmap it.
				// extmmState.deallocate(entry->address, entry->numPages);
				// vmm::unmapAddress(VirtAddr(entry->address), entry->numPages, /* ignoreUnmapped: */ true);

				// for safety, make sure we don't touch this again (accidentally or otherwise)
				entry->memoryType = MemoryType::Reserved;
			}
		}
	}

	// this one frees the memory map, and the bootinfo itself.
	void freeAllEarlyMemory(BootInfo* bootinfo)
	{
		uint64_t maxIdentMem = bootinfo->maximumIdentityMap;

		// TOOD: investigate why tf we have some issues when we add memory around 0x7a00000 ~ 0x7f00000 to the pool
		// resulting in massive slowdowns. currently we only get that area if the system memory is set to 128MB,
		// but it's too much effort to allow allocating specific chunks in the extmm. (if we set sysmem to 256MB, then
		// we don't get entries starting around 0x7a00000 in the memory map)

		// first make a copy, because we are about to deallocate and unmap the bootinfo memory that
		// the memory map sits on, so it would be prudent to make sure we have a copy of it first.
		auto numEntries = bootinfo->mmEntryCount;
		auto entriesCopy = new MemMapEntry[bootinfo->mmEntryCount];
		memcpy(entriesCopy, bootinfo->mmEntries, sizeof(MemMapEntry) * numEntries);

		// unmap any extraneous bits and pieces
		for(size_t i = 0; i < numEntries; i++)
		{
			auto entry = &entriesCopy[i];
			if(entry->address >= maxIdentMem && (entry->memoryType == MemoryType::AvailableAfterUnmap
				|| entry->memoryType == MemoryType::MemoryMap))
			{
				totalPhysicalMemory += entry->numPages * 0x1000;

				extmmState.deallocate(entry->address, entry->numPages);
				vmm::unmapAddress(VirtAddr(entry->address), entry->numPages, /* ignoreUnmapped: */ true);
			}
		}

		// now get rid of the copy, for obvious reasons.
		delete[] entriesCopy;

		// ask the vmm to unmap all identity memory.
		vmm::unmapAddress(VirtAddr::zero(), maxIdentMem / PAGE_SIZE, /* ignoreUnmapped: */ true);

		auto addr = (addr_t) bootinfo;
		assert(isPageAligned(addr));

		// we assume the kernel boot info is one page long!!
		deallocate(PhysAddr(addr), 1);

		{
			auto num = totalPhysicalMemory / (1024 * 1024); int unit = 0;
			char units[] = { 'M', 'G', 'T', 'P' };

			while(num >= 1024)
				num /= 1024, unit++;

			log("pmm", "freed all bootloader memory: {} bytes ({} {}B)", totalPhysicalMemory, num, units[unit]);
		}

		// at this point, check for consistency
		if(!extmmState.checkConsistency())
			abort("pmm inconsistent!");

		numAllocatedBytes = 0;
	}


	PhysAddr allocate(size_t num, bool below4G)
	{
		// extmmState.dump();
		auto ret = extmmState.allocate(num, below4G ? [](addr_t a, size_t l) -> bool {
			return end(a, l) < 0xFFFF'FFFF;
		} : [](addr_t, size_t) -> bool { return true; });

		assert(ret);

		// log("pmm", "allocated {p} - {p} ({}) ({})", ret, ret + num * PAGE_SIZE, num, extmmState.numExtents);
		numAllocatedBytes += num * PAGE_SIZE;
		return PhysAddr(ret);
	}

	void deallocate(PhysAddr addr, size_t num)
	{
		assert(addr.nonZero());
		assert(num);

		assert(addr.isPageAligned());
		if(addr == zeroPageAddr && num == 1)
			return;

		// log("pmm", "deallocated {p} - {p} ({})", addr.ptr(), addr + ofsPages(num), num);
		extmmState.deallocate(addr.addr(), num);
		numAllocatedBytes -= num * PAGE_SIZE;
	}
}
}



































