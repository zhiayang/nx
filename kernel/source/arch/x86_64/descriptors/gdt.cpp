// gdt.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace cpu {
namespace gdt
{
	struct entry_t
	{
		uint16_t limit_low;
		uint16_t base_low;

		uint8_t  base_mid;
		uint8_t  access;
		uint8_t  flags;
		uint8_t  base_high;

	} __attribute__((packed));

	struct gdtptr_t
	{
		uint16_t limit;
		uint64_t base;

	} __attribute__((packed));

	constexpr entry_t makeGDTEntry(uint32_t base, uint32_t limit, bool code, int ring)
	{
		return entry_t {
			.limit_low  = (uint16_t) (limit & 0xFFFF),
			.base_low   = (uint16_t) (base & 0xFFFF),
			.base_mid   = (uint8_t)  ((base & 0xFF0000) >> 16),
			.access     = (uint8_t)  (0x92 | (code ? 0x8 : 0) | ((ring & 0x3) << 5)),
			.flags      = (uint8_t)  ((limit & 0xF0000) >> 16 | (1 << 5)),
			.base_high  = (uint8_t)  ((base & 0xFF000000) >> 24)
		};
	}

	// access setup:
	// 0: accessed (useless)
	// 1: readable (useless) -- but people complain when it's not set, so we set it.
	// 2: conforming (should be kept at 0)
	// 3: 1 for code, 0 for data
	// 4: 1 for code/data, 0 for tss
	// 5:6: privilege level
	// 7: present

	// flags setup:
	// 0:3: segment limit (useless)
	// 4: avail (useless)
	// 5: long mode (always 1)
	// 6: d (always 0)
	// 7: granularity (useless, 0)

	extern "C" void nx_x64_loadgdt(uint64_t);

	static addr_t GDTBaseAddress = 0;
	static addr_t GDTPhysAddress = 0;
	static size_t CurrentGDTSize = 0;

	static gdtptr_t GDTPointer;

	static bool HasFSGSBaseInstr = false;


	static int addDescriptor(const entry_t& entry)
	{
		*((entry_t*) (GDTBaseAddress + CurrentGDTSize)) = entry;

		auto ret = CurrentGDTSize;

		CurrentGDTSize += sizeof(entry_t);
		return ret;
	}

	krt::pair<addr_t, addr_t> getGDTAddress()
	{
		return krt::pair(GDTBaseAddress, GDTPhysAddress);
	}

	size_t getNextSelectorIndex()
	{
		return CurrentGDTSize;
	}

	void incrementSelectorIndex(size_t ofs)
	{
		CurrentGDTSize += ofs;
		if(CurrentGDTSize >= 0xFFFF)
			abort("gdt is out of space!!!");

		// reload the gdt with the new limit.
		GDTPointer.limit = CurrentGDTSize - 1;
		nx_x64_loadgdt((addr_t) &GDTPointer);
	}

	void init()
	{
		// allocate me a page.
		{
			GDTPhysAddress = pmm::allocate(1);
			GDTBaseAddress = vmm::allocateAddrSpace(1, vmm::AddressSpace::Kernel);

			vmm::mapAddress(GDTBaseAddress, GDTPhysAddress, 1, vmm::PAGE_PRESENT | vmm::PAGE_WRITE);
			memset((void*) GDTBaseAddress, 0, PAGE_SIZE);
		}


		// add the intial entries:
		{
			addDescriptor(makeGDTEntry(0, 0, 0, 0));                // null descriptor

			addDescriptor(makeGDTEntry(0, 0xFFFFFFFF, true, 0));    // 0x08: ring 0 code segment
			addDescriptor(makeGDTEntry(0, 0xFFFFFFFF, false, 0));   // 0x10: ring 0 data segment

			// apparently sysret "expects" a 32-bit ring-3 code descriptor in this position (ie. before the ring 3 data segment)
			// since we don't give a shit about compat-mode, we just put a null descriptor here.

			addDescriptor(makeGDTEntry(0, 0, 0, 0));                // 0x18: stupid null descriptor

			addDescriptor(makeGDTEntry(0, 0xFFFFFFFF, false, 3));   // 0x20: ring 3 data segment
			addDescriptor(makeGDTEntry(0, 0xFFFFFFFF, true, 3));    // 0x18: ring 3 code segment
		}


		GDTPointer.base = GDTBaseAddress;
		GDTPointer.limit = CurrentGDTSize - 1;

		nx_x64_loadgdt((addr_t) &GDTPointer);

		HasFSGSBaseInstr = cpu::hasFeature(Feature::FSGSBase);
		if(HasFSGSBaseInstr)
		{
			writeCR4(readCR4() | CR4_FSGSBASE);
		}
	}
}


	uint64_t readFSBase()
	{
		if(__likely(gdt::HasFSGSBaseInstr))
		{
			uint64_t base;
			asm volatile ("rdfsbase %0" : "=a"(base));
			return base;
		}
		else
		{
			return readMSR(MSR_FS_BASE);
		}
	}

	void writeFSBase(uint64_t base)
	{
		if(__likely(gdt::HasFSGSBaseInstr))
		{
			asm volatile ("wrfsbase %0" :: "a"(base));
		}
		else
		{
			writeMSR(MSR_FS_BASE, base);
		}
	}
}
}






























