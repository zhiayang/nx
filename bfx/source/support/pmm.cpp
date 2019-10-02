// pmm.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "bfx.h"
#include "bootboot.h"

namespace bfx {
namespace extmm
{
	struct extent_t
	{
		uint64_t addr;
		uint64_t size;

		extent_t* next = 0;
	};

	struct State
	{
		extent_t* head = 0;
		size_t numExtents = 0;

		const char* owner = 0;

		uint64_t bootstrapStart = 0;
		uint64_t bootstrapWatermark = 0;
		uint64_t bootstrapEnd = 0;
	};

	static uint64_t end(uint64_t base, size_t num)  { return base + (num * PAGE_SIZE); }
	static uint64_t end(extent_t* ext)              { return ext->addr + (ext->size * PAGE_SIZE); }

	static void addExtent(State* st, uint64_t addr, size_t size)
	{
		extent_t* ext = 0;
		if(st->bootstrapWatermark + sizeof(extent_t) <= st->bootstrapEnd)
		{
			ext = (extent_t*) st->bootstrapWatermark;
			st->bootstrapWatermark += sizeof(extent_t);
		}
		else
		{
			// we're out of space, lmao.
			return;

			// ext = new extent_t();
		}

		ext->addr = addr;
		ext->size = size;

		// insert at the head.
		ext->next = st->head;

		st->head = ext;
		st->numExtents += 1;
	}

	void init(State* st, const char* owner, uint64_t base, uint64_t top)
	{
		st->head = 0;
		st->numExtents = 0;

		st->owner = owner;

		st->bootstrapWatermark = base;
		st->bootstrapStart = base;
		st->bootstrapEnd = top;
	}

	uint64_t allocate(State* st, size_t num, bool (*satisfies)(uint64_t, size_t))
	{
		if(num == 0) abort("extmm/%s::allocate(): cannot allocate 0 pages!", st->owner);

		auto ext = st->head;
		while(ext)
		{
			if(ext->size >= num && satisfies(ext->addr, ext->size))
			{
				// return this.
				// take from the bottom.

				uint64_t ret = ext->addr;
				ext->addr += (num * PAGE_SIZE);
				ext->size -= num;

				return ret;
			}

			ext = ext->next;
		}

		println("extmm/%s::allocate(): out of pages!", st->owner);
		return 0;
	}

	void deallocate(State* st, uint64_t addr, size_t num)
	{
		// loop through every extent. we can match in two cases:
		// 1. 'addr' is 'num' pages *below* the base of the extent
		// 2. 'addr' is *size* pages above the *end* of the extent

		auto ext = st->head;
		while(ext)
		{
			if(end(addr, num) == ext->addr)
			{
				// we are below. decrease the addr of the extent by num pages.
				ext->addr -= (PAGE_SIZE * num);
				return;
			}
			else if(end(ext) == addr)
			{
				// increase the size only
				ext->size += num;
				return;
			}

			ext = ext->next;
		}

		// oops. make a new extent.
		addExtent(st, addr, num);
	}
}

namespace pmm
{
	// this is the number of pages that we'll use for our bootstrap of the memory manager.
	constexpr size_t NumReservedPages = 8;

	static uint64_t end(uint64_t base, size_t num)  { return base + (num * PAGE_SIZE); }

	static extmm::State extmmState;

	constexpr uintptr_t PMM_STACK_BASE  = 0xFFFF'FFFF'F3C0'0000;
	constexpr uintptr_t PMM_STACK_END   = 0xFFFF'FFFF'F3D0'0000;

	void init(MMapEnt* ents, size_t numEnts)
	{
		// find a bootstrappable chunk of memory marked as usable.
		bool bootstrapped = false;
		{
			for(size_t i = 0; i < numEnts; i++)
			{
				// idk what is going on, but using low memory fucks us up big time.
				if(MMapEnt_Type(&ents[i]) == MMAP_FREE && MMapEnt_Size(&ents[i]) / PAGE_SIZE >= NumReservedPages
					&& MMapEnt_Ptr(&ents[i]) < 0xFFFF'FFFF && MMapEnt_Ptr(&ents[i]) >= 0x100000)
				{
					// ok. modify the map first to steal the memory:
					uint64_t base = MMapEnt_Ptr(&ents[i]);

					ents[i].ptr += (NumReservedPages * PAGE_SIZE);
					ents[i].size -= (PAGE_SIZE * NumReservedPages);

					// bootstrap!
					vmm::bootstrap(base, PMM_STACK_BASE, NumReservedPages);
					bootstrapped = true;
					break;
				}
			}
		}

		if(!bootstrapped) abort("failed to bootstrap pmm!!");

		// now that we have bootstrapped one starting page for ourselves, we can init the ext mm.
		extmm::init(&extmmState, "pmm", PMM_STACK_BASE, PMM_STACK_END);

		// ok, now loop through each memory entry for real.
		size_t totalMem = 0;
		for(size_t i = 0; i < numEnts; i++)
		{
			auto entry = &ents[i];
			if(MMapEnt_Type(entry) == MMAP_FREE)
			{
				// println("%p - %p", MMapEnt_Ptr(entry), MMapEnt_Ptr(entry) + MMapEnt_Size(entry));

				extmm::deallocate(&extmmState, MMapEnt_Ptr(entry), MMapEnt_Size(entry) / PAGE_SIZE);
				totalMem += MMapEnt_Size(entry);
			}
		}
	}

	uint64_t allocate(size_t num, bool below4G)
	{
		// lol. we can't have capturing lambdas without some kind of std::function
		// and nobody knows how to implement that shit.
		return extmm::allocate(&extmmState, num, below4G ? [](uint64_t a, size_t l) -> bool {
			return end(a, l) < 0xFFFF'FFFF;
		} : [](uint64_t, size_t) -> bool { return true; });
	}
}


namespace mmap
{
	using mmapentry_t = nx::MemMapEntry;

	static mmapentry_t* entryArray  = 0;
	static uint64_t entryArrayPages = 0;

	static size_t numEntries = 0;
	static size_t maxEntries = 0;

	void init(MMapEnt* ents, size_t bbents)
	{
		// loop through all the entries first.
		size_t neededEnts = 0;

		for(size_t i = 0; i < bbents; i++)
		{
			if(MMapEnt_Type(&ents[i]) == MMAP_ACPI || MMapEnt_Type(&ents[i]) == MMAP_MMIO)
				neededEnts++;
		}

		// add all the free memory. good thing pmm is in the same file.
		// note: we never free memory, so the number of extents will never increase.
		neededEnts += pmm::extmmState.numExtents;

		// add extents for the bootinfo, memory map, loaded kernel, framebuffer and initrd:
		// (they will be contiguous, so one extent each is sufficient)
		neededEnts += 5;

		entryArrayPages = (neededEnts * sizeof(mmapentry_t) + PAGE_SIZE - 1) / PAGE_SIZE;
		entryArray      = (mmapentry_t*) pmm::allocate(entryArrayPages);

		// add the acpi and mmio regions first, cos those won't change:
		for(size_t i = 0; i < bbents; i++)
		{
			uint64_t memtype = 0;

			if(MMapEnt_Type(&ents[i]) == MMAP_ACPI)
				memtype = (uint64_t) nx::MemoryType::ACPI;

			else if(MMapEnt_Type(&ents[i]) == MMAP_MMIO)
				memtype = (uint64_t) nx::MemoryType::MMIO;

			if(memtype != 0)
			{
				entryArray[numEntries] = {
					.address    = MMapEnt_Ptr(&ents[i]),
					.numPages   = MMapEnt_Size(&ents[i]) / PAGE_SIZE,
					.memoryType = (nx::MemoryType) memtype
				};
				numEntries++;
			}
		}

		maxEntries = (entryArrayPages * PAGE_SIZE) / sizeof(mmapentry_t);
	}

	void addEntry(uint64_t base, size_t pages, uint64_t type)
	{
		if(numEntries == maxEntries)
			abort("too many mmap entries!");

		entryArray[numEntries] = {
			.address    = base,
			.numPages   = pages,
			.memoryType = (nx::MemoryType) type
		};

		numEntries++;
	}

	void finalise(nx::BootInfo* bi)
	{
		// all we need to do now is to add the pmm extents.
		auto ext = pmm::extmmState.head;
		while(ext)
		{
			addEntry(ext->addr, ext->size, (uint64_t) nx::MemoryType::Available);
			ext = ext->next;
		}

		addEntry((uint64_t) entryArray, entryArrayPages, (uint64_t) nx::MemoryType::MemoryMap);
		vmm::mapKernel((uint64_t) entryArray, (uint64_t) entryArray, entryArrayPages, 0);

		// must make all the entries page-aligned!
		for(size_t i = 0; i < numEntries; i++)
		{
			auto ent = &entryArray[i];

			// numpages was already rounded down, so we just need to round up the address.
			ent->address = (ent->address + 0xFFF) & 0xFFFF'FFFF'FFFF'F000;
		}

		// setup the memory map:
		bi->mmEntries = entryArray;
		bi->mmEntryCount = numEntries;

		println("created kernel memory map, with %d entries", numEntries);
	}
}
}








