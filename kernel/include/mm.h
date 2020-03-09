// mm.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include "defs.h"
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

		addr_t allocate(size_t num, bool below4G = false);
		void deallocate(addr_t addr, size_t num);

		void freeEarlyMemory(BootInfo* bootinfo, MemoryType type);
		void freeAllEarlyMemory(BootInfo* bootinfo);

		// this returns the single, 0-filled page we use for CoW and lazy allocation purposes.
		addr_t getZeroPage();
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
		void mapAddressOverwrite(addr_t virt, addr_t phys, size_t num, uint64_t flags, scheduler::Process* proc = 0);

		// this one will abort if you try to do anything funny.
		void mapAddress(addr_t virt, addr_t phys, size_t num, uint64_t flags, scheduler::Process* proc = 0);

		void mapCOW(addr_t virt, addr_t phys, size_t num, uint64_t flags, scheduler::Process* proc = 0);
		void mapLazy(addr_t virt, size_t num, uint64_t flags, scheduler::Process* proc = 0);


		uint64_t getPageFlags(addr_t virt, scheduler::Process* proc = 0);
		void unmapAddress(addr_t virt, size_t num, bool freePhys, bool ignoreIfNotMapped = false, scheduler::Process* proc = 0);
		addr_t getPhysAddr(addr_t virt, scheduler::Process* proc = 0);
		bool isMapped(addr_t virt, size_t num, scheduler::Process* proc = 0);



		// vmm.cpp stuff -- handles address space allocation + physical page allocation.
		enum class AddressSpaceType { Kernel, KernelHeap, User };

		addr_t allocateAddrSpace(size_t num, AddressSpaceType type, scheduler::Process* proc = 0);
		void deallocateAddrSpace(addr_t addr, size_t num, scheduler::Process* proc = 0);

		addr_t allocateSpecific(addr_t addr, size_t num, scheduler::Process* proc = 0);

		// this will allocate a region of memory, and map the physical pages LAZILY
		addr_t allocate(size_t num, AddressSpaceType type, uint64_t flags = 0, scheduler::Process* proc = 0);
		addr_t allocateEager(size_t num, AddressSpaceType type, uint64_t flags = 0, scheduler::Process* proc = 0);

		void deallocate(addr_t addr, size_t num, scheduler::Process* proc = 0);


		bool handlePageFault(uint64_t cr2, uint64_t errorCode, uint64_t rip);


		constexpr size_t indexPML4(addr_t addr)       { return ((((addr_t) addr) >> 39) & 0x1FF); }
		constexpr size_t indexPDPT(addr_t addr)       { return ((((addr_t) addr) >> 30) & 0x1FF); }
		constexpr size_t indexPageDir(addr_t addr)    { return ((((addr_t) addr) >> 21) & 0x1FF); }
		constexpr size_t indexPageTable(addr_t addr)  { return ((((addr_t) addr) >> 12) & 0x1FF); }

		constexpr addr_t indexToAddr(size_t p4idx, size_t p3idx, size_t p2idx, size_t p1idx)
		{
			return (0x80'0000'0000ULL * p4idx) + (0x4000'0000ULL * p3idx) + (0x20'0000ULL * p2idx) + (0x1000ULL * p1idx);
		}

		constexpr size_t NumAddressSpaces = 3;
		constexpr addr_t AddressSpaces[NumAddressSpaces][2] = {
			{ addrs::USER_ADDRSPACE_BASE,       addrs::USER_ADDRSPACE_END       },
			{ addrs::KERNEL_HEAP_BASE,          addrs::KERNEL_HEAP_END          },
			{ addrs::KERNEL_VMM_ADDRSPACE_BASE, addrs::KERNEL_VMM_ADDRSPACE_END }
		};

		constexpr addr_t VMMStackAddresses[NumAddressSpaces][2] = {
			{ addrs::VMM_STACK0_BASE,   addrs::VMM_STACK0_END },
			{ addrs::VMM_STACK1_BASE,   addrs::VMM_STACK1_END },
			{ addrs::VMM_STACK2_BASE,   addrs::VMM_STACK2_END }
		};


		constexpr uint64_t PAGE_PRESENT         = 0x1;
		constexpr uint64_t PAGE_WRITE           = 0x2;
		constexpr uint64_t PAGE_USER            = 0x4;
		constexpr uint64_t PAGE_COPY_ON_WRITE   = 0x200;
		constexpr uint64_t PAGE_NX              = 0x8000'0000'0000'0000;

		constexpr addr_t PAGE_ALIGN(addr_t addr)
		{
			// check if we have the 62-nd bit set, to determine if we actually had the uppermost bit set.
			// obviously when the virtual address space increases to 62-bits or more from 48-bits, then
			// this ain't gonna work.
			if(addr & 0x4000'0000'0000'0000)    return addr & 0xFFFF'FFFF'FFFF'F000;
			else                                return addr & 0x7FFF'FFFF'FFFF'F000;
		}

		constexpr bool isAligned(addr_t addr)
		{
			return addr == (addr & ~((addr_t) 0xFFF));
		}


		struct VMRegion
		{
			VMRegion(addr_t addr, size_t num);

			addr_t addr;
			size_t numPages;

			// it's a dynamically allocated fixed size array, if that makes sense.
			// the phys page of addr is in backing[0], addr + 0x1000 is in backing[1], etc.
			nx::array<addr_t> backingPhysPages;
		};

		struct AddressSpace
		{
			addr_t cr3;
			nx::list<VMRegion> regions;
			nx::array<addr_t> allocatedPhysPages;

			void init(addr_t cr3 = 0);
			void destroy();

			void addRegion(addr_t addr, size_t size);
			void freeRegion(addr_t addr, size_t size);

			void addPhysicalMapping(addr_t virt, addr_t phys);
		};
	}

	constexpr size_t SIZE_IN_PAGES(size_t bytes)
	{
		return (bytes + (PAGE_SIZE - 1)) / PAGE_SIZE;
	}



}












