// mm.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include "defs.h"
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
	}

	namespace pmm
	{
		void init(BootInfo* bootinfo);

		addr_t allocate(size_t num, bool below4G = false);
		void deallocate(addr_t addr, size_t num);

		void freeEarlyMemory(BootInfo* bootinfo, MemoryType type);
		void freeAllEarlyMemory(BootInfo* bootinfo);
	}

	namespace vmm
	{
		struct pml_t
		{
			uint64_t entries[512];
		};

		void setupAddrSpace(scheduler::Process* proc);
		void bootstrap(addr_t physBase, addr_t virt, size_t maxPages);
		void init(scheduler::Process* proc);


		void invalidate(addr_t addr);
		void invalidateAll();

		void mapAddress(addr_t virt, addr_t phys, size_t num, uint64_t flags, scheduler::Process* proc = 0);
		void unmapAddress(addr_t virt, size_t num, bool freePhys, scheduler::Process* proc = 0);
		addr_t getPhysAddr(addr_t virt, scheduler::Process* proc = 0);

		// functions to allocate/free address space in higher half (kernel) and lower half (user)
		enum class AddressSpace { Kernel, KernelHeap, User };

		addr_t allocateAddrSpace(size_t num, AddressSpace type, scheduler::Process* proc = 0);
		void deallocateAddrSpace(addr_t addr, size_t num, scheduler::Process* proc = 0);

		addr_t allocateSpecific(addr_t addr, size_t num, scheduler::Process* proc = 0);

		// these will allocate an address space, *AND* allocate a physical page *AND* map it!
		addr_t allocate(size_t num, AddressSpace type, scheduler::Process* proc = 0);
		void deallocate(addr_t addr, size_t num, scheduler::Process* proc = 0);

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


		constexpr uint64_t PAGE_PRESENT     = 0x1;
		constexpr uint64_t PAGE_WRITE       = 0x2;
		constexpr uint64_t PAGE_USER        = 0x4;
		constexpr uint64_t PAGE_NX          = 0x8000'0000'0000'0000;

		constexpr uint64_t PAGE_ALIGN       = ~0xFFF;
	}




	namespace extmm
	{
		struct extent_t;
		struct State
		{
			extent_t* head = 0;
			size_t numExtents = 0;

			const char* owner = 0;

			addr_t bootstrapWatermark = 0;
			addr_t bootstrapEnd = 0;
		};

		void dump(State* st);

		void init(State* st, const char* owner, addr_t baseAddr, addr_t maxAddr);

		addr_t allocate(State* state, size_t num, bool (*satisfies)(addr_t, size_t));
		addr_t allocateSpecific(State* state, addr_t start, size_t num);

		void deallocate(State* state, addr_t addr, size_t num);
	}

}












