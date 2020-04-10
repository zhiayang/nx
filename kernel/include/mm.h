// mm.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include "defs.h"
#include "extmm.h"
#include "synchro.h"
#include "misc/addrs.h"

namespace nx
{
	struct BootInfo;
	enum class MemoryType : uint64_t;

	namespace heap
	{
		void init();
		addr_t allocate(size_t size, size_t align);
		void deallocate(addr_t addr);

		bool initialised();

		size_t getNumAllocatedBytes();
	}

	// this is just a separate copy of 'heap' that never locks, only for use
	// in IRQ contexts.
	namespace fixed_heap
	{
		void init();
		addr_t allocate(size_t size, size_t align);
		void deallocate(addr_t addr);

		bool initialised();

		size_t getNumAllocatedBytes();
	}

	namespace pmm
	{
		void init(BootInfo* bootinfo);

		PhysAddr allocate(size_t num, bool below4G = false);
		void deallocate(PhysAddr addr, size_t num);

		void freeEarlyMemory(BootInfo* bootinfo, MemoryType type);
		void freeAllEarlyMemory(BootInfo* bootinfo);

		// this returns the single, 0-filled page we use for CoW and lazy allocation purposes.
		PhysAddr getZeroPage();
	}

	namespace vmm
	{
		struct pml_t
		{
			uint64_t entries[512];
		};

		// virtmap.cpp stuff
		void setupAddrSpace(scheduler::Process* proc);
		void bootstrap(addr_t physBase, addr_t virt, size_t maxPages);

		void init(scheduler::Process* proc);
		void destroy(scheduler::Process* proc);

		void switchAddrSpace(addr_t cr3);

		void invalidate(addr_t addr);
		void invalidateAll();

		// this one overwrites whatever physical page was there.
		void mapAddressOverwrite(VirtAddr virt, PhysAddr phys, size_t num, uint64_t flags, scheduler::Process* proc = 0);

		// this one will abort if you try to do anything funny.
		void mapAddress(VirtAddr virt, PhysAddr phys, size_t num, uint64_t flags, scheduler::Process* proc = 0);

		void mapCOW(VirtAddr virt, PhysAddr phys, size_t num, uint64_t flags, scheduler::Process* proc = 0);
		void mapLazy(VirtAddr virt, size_t num, uint64_t flags, scheduler::Process* proc = 0);


		uint64_t getPageFlags(VirtAddr virt, scheduler::Process* proc = 0);
		void unmapAddress(VirtAddr virt, size_t num, bool ignoreIfNotMapped = false, scheduler::Process* proc = 0);
		PhysAddr getPhysAddr(VirtAddr virt, scheduler::Process* proc = 0);
		bool isMapped(VirtAddr virt, size_t num, scheduler::Process* proc = 0);



		// vmm.cpp stuff -- handles address space allocation + physical page allocation.
		enum class AddressSpaceType { Kernel, KernelHeap, User };

		VirtAddr allocateAddrSpace(size_t num, AddressSpaceType type, scheduler::Process* proc = 0);
		void deallocateAddrSpace(VirtAddr addr, size_t num, scheduler::Process* proc = 0);

		VirtAddr allocateSpecific(VirtAddr addr, size_t num, scheduler::Process* proc = 0);

		// this will allocate a region of memory, and map the physical pages LAZILY
		VirtAddr allocate(size_t num, AddressSpaceType type, uint64_t flags = 0, scheduler::Process* proc = 0);
		VirtAddr allocateEager(size_t num, AddressSpaceType type, uint64_t flags = 0, scheduler::Process* proc = 0);

		void deallocate(VirtAddr addr, size_t num, scheduler::Process* proc = 0);


		bool handlePageFault(uint64_t cr2, uint64_t errorCode, uint64_t rip);

		constexpr uint64_t PAGE_PRESENT         = 0x1;
		constexpr uint64_t PAGE_WRITE           = 0x2;
		constexpr uint64_t PAGE_USER            = 0x4;
		constexpr uint64_t PAGE_COPY_ON_WRITE   = 0x200;
		constexpr uint64_t PAGE_NX              = 0x8000'0000'0000'0000;
		constexpr uint64_t ALIGN_BITS           = 12;


		struct PageHasher
		{
			size_t operator () (VirtAddr v) const
			{
				return v.addr() >> ALIGN_BITS;
			}
		};

		struct VMRegion
		{
			VMRegion(VirtAddr addr, size_t num);
			VMRegion(const VMRegion&);
			VMRegion(VMRegion&&);

			VMRegion& operator = (VMRegion&&);
			VMRegion& operator = (const VMRegion&);

			void grow_up(size_t count);     // addr remains the same
			void grow_down(size_t count);   // addr decreases

			void shrink_up(size_t count);   // addr increases
			void shrink_down(size_t count); // addr remains the same

			VirtAddr end() const;


			VirtAddr addr;
			size_t numPages;

			nx::bucket_hashmap<VirtAddr, PhysAddr, PageHasher> backingPhysPages;
		};

		constexpr size_t NumAddressSpaces = 3;
		struct AddressSpace
		{
			PhysAddr cr3;
			nx::array<VMRegion> regions;
			nx::array<PhysAddr> allocatedPhysPages;

			extmm::State<> vmmStates[vmm::NumAddressSpaces];

			void init(PhysAddr cr3 = PhysAddr::zero());
			void destroy();

			void addRegion(VirtAddr addr, size_t size);
			void addRegion(VirtAddr virtStart, PhysAddr physStart, size_t size);

			void freeRegion(VirtAddr addr, size_t size, bool freePhys);

			addr_t addPhysicalMapping(VirtAddr virt, PhysAddr phys);
			// void addPhysicalMapping(addr_t virt, addr_t phys, size_t num);
		};
	}

	constexpr size_t SIZE_IN_PAGES(size_t bytes)
	{
		return (bytes + (PAGE_SIZE - 1)) / PAGE_SIZE;
	}



}












