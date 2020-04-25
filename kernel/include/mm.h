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

		// this one maps the pages as copy-on-write
		void mapCOW(VirtAddr virt, PhysAddr phys, size_t num, uint64_t flags, scheduler::Process* proc = 0);

		// this one maps them as copy-on-write as well, but all the pages are zero -- used mainly for mmaped memory
		void mapZeroedCOW(VirtAddr virt, size_t num, uint64_t flags, scheduler::Process* proc = 0);

		// this one maps them as lazy, ie. on both reads and writes, the #PF handler will fetch the physical page
		// from the process AddressSpace to figure out the correct thing to map.
		void mapLazy(VirtAddr virt, size_t num, uint64_t flags, scheduler::Process* proc = 0);

		uint64_t getPageFlags(VirtAddr virt, scheduler::Process* proc = 0);
		void unmapAddress(VirtAddr virt, size_t num, bool ignoreIfNotMapped = false, scheduler::Process* proc = 0);
		PhysAddr getPhysAddr(VirtAddr virt, scheduler::Process* proc = 0);
		bool isMapped(VirtAddr virt, size_t num, scheduler::Process* proc = 0);



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
			VMRegion(VMRegion&&);
			VMRegion& operator = (VMRegion&&);

			VMRegion(const VMRegion&) = delete;
			VMRegion& operator = (const VMRegion&) = delete;

			void grow_up(size_t count);     // addr remains the same
			void grow_down(size_t count);   // addr decreases

			void shrink_up(size_t count);   // addr increases
			void shrink_down(size_t count); // addr remains the same

			VirtAddr end() const;
			VMRegion clone() const;

			optional<PhysAddr> lookup(VirtAddr virt) const;

			VirtAddr addr;
			size_t numPages;

			nx::bucket_hashmap<VirtAddr, PhysAddr, PageHasher> backingPhysPages;
		};

		// vmregion for shared memory, where the record of physical pages is stored in a shared manner.
		// the important point is that the virtual region is free to change between processes, but the
		// backing physical pages must still be fixed. this means that we can't use a hashmap from virt->phys
		// but instead use the "index" from the base virtual address. we must still use a hashmap because it
		// needs to be a sparse record. we also don't need to implement any of the grow/shrink operations,
		// since this region must be "dealt with" in totality.
		struct SharedVMRegion
		{
			struct SharedPhysRegion
			{
				mutex mtx;
				nx::bucket_hashmap<size_t, PhysAddr> physPages;

				void destroy();
			};


			SharedVMRegion(VirtAddr addr, size_t num, SharedPhysRegion* store);
			SharedVMRegion(SharedVMRegion&&);
			SharedVMRegion& operator = (SharedVMRegion&&);

			SharedVMRegion(const SharedVMRegion&) = delete;
			SharedVMRegion& operator = (const SharedVMRegion&) = delete;

			VirtAddr end() const;
			SharedVMRegion clone() const;

			optional<PhysAddr> lookup(VirtAddr virt) const;

			bool operator == (const SharedVMRegion& oth) const { return this->addr == oth.addr && this->numPages == oth.numPages && this->backingPhysPages == oth.backingPhysPages; }

			VirtAddr addr;
			size_t numPages;

			SharedPhysRegion* backingPhysPages;
		};

		/*
			this needs special treatment, because of the shitty way that the kernel is designed.

			because the AddressSpace of a process holds a lot of information, it is not feasible to
			lock the entire structure when we need to change something. the biggest issue is that
			the vmmStates have their own lock, and do not need the main addressspace to be locked
			to manipulate the allocations.

			so, instead of wrapping the AddressSpace in a Synchronised<>, we will manually implement
			those semantics here; certain methods (eg. those that manipulate regions) will only be
			callable from a lock()-ed addressspace, while others (eg. cr3(), vmmStates()) can be
			called on a normal, non-locked instance.
		*/
		struct AddressSpace
		{
			// these are accessible without a lock.
			PhysAddr cr3();
			extmm::State<>& getVMMState(AddressSpaceType addrspc);

		private:
			PhysAddr theCR3;
			nx::array<VMRegion> regions;
			nx::array<SharedVMRegion> sharedRegions;

			extmm::State<> vmmStates[vmm::NumAddressSpaces];

			// bookkeeping for miscellaneous physical pages that we allocate (eg. for page tables)
			nx::array<PhysAddr> allocatedPhysPages;

			// this is a spinlock! see the paragraphs in _heap_impl.h on why.
			nx::spinlock lk;

			struct LockedAddrSpace
			{
				~LockedAddrSpace() { this->addrspace.lk.unlock(); }
				LockedAddrSpace* operator -> () { return this; }

				// here are the methods only accessible from a locked state

				void init(PhysAddr cr3 = PhysAddr::zero());
				void destroy();

				void addRegion(VirtAddr addr, size_t size);
				void addRegion(VirtAddr virtStart, PhysAddr physStart, size_t size);

				void addSharedRegion(SharedVMRegion&& region);
				void removeSharedRegion(SharedVMRegion&& region);

				void freeRegion(VirtAddr addr, size_t size, bool freePhys);

				PhysAddr addPhysicalMapping(VirtAddr virt, PhysAddr phys);

				// if the address was inside the addressspace mapping, then the VirtAddr returned in the
				// optional will be the same as the input addr. If there is an existing physical mapping
				// for it also, then the second optional will contain that.
				krt::pair<optional<VirtAddr>, optional<PhysAddr>> lookupVirtualMapping(VirtAddr addr);

				void addTrackedPhysPage(PhysAddr addr);
				void addTrackedPhysPages(PhysAddr base, size_t count);

				void removeTrackedPhysPage(PhysAddr addr);
				void removeTrackedPhysPages(PhysAddr base, size_t count);

			private:
				LockedAddrSpace(AddressSpace& sync) : addrspace(sync) { this->addrspace.lk.lock(); }

				LockedAddrSpace(LockedAddrSpace&&) = delete;
				LockedAddrSpace(const LockedAddrSpace&) = delete;

				LockedAddrSpace& operator = (LockedAddrSpace&&) = delete;
				LockedAddrSpace& operator = (const LockedAddrSpace&) = delete;

				AddressSpace& addrspace;

				friend struct AddressSpace;
			};

		public:

			LockedAddrSpace lock() { return LockedAddrSpace(*this); }
			bool isLocked() { return this->lk.held(); }
		};



		VirtAddr allocateAddrSpace(size_t num, AddressSpaceType type, scheduler::Process* proc = 0);
		void deallocateAddrSpace(VirtAddr addr, size_t num, scheduler::Process* proc = 0);

		VirtAddr allocateSpecific(VirtAddr addr, size_t num, scheduler::Process* proc = 0);
		VirtAddr allocate(size_t num, AddressSpaceType type, uint64_t flags = 0, scheduler::Process* proc = 0);
		VirtAddr allocateEager(size_t num, AddressSpaceType type, uint64_t flags = 0, scheduler::Process* proc = 0);

		void deallocate(VirtAddr addr, size_t num, scheduler::Process* proc = 0);







		bool handlePageFault(uint64_t cr2, uint64_t errorCode, uint64_t rip);
	}

	constexpr size_t SIZE_IN_PAGES(size_t bytes)
	{
		return (bytes + (PAGE_SIZE - 1)) / PAGE_SIZE;
	}



}












