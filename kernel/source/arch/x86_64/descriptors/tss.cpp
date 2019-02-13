// tss.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace cpu {
namespace tss
{
	struct system_segment_t
	{
		uint16_t limit_low;
		uint16_t base_low;

		uint8_t  base_mid;
		uint8_t  type;
		uint8_t  flags;
		uint8_t  base_high;

		uint32_t base_highest;
		uint32_t reserved;

	} __attribute__((packed));

	struct tss_t
	{
		// idk why it's not aligned, ask amd
		uint32_t reserved0;

		uint64_t rsp0;
		uint64_t rsp1;
		uint64_t rsp2;

		uint64_t reserved1;

		uint64_t ist1;
		uint64_t ist2;
		uint64_t ist3;
		uint64_t ist4;
		uint64_t ist5;
		uint64_t ist6;
		uint64_t ist7;

		uint64_t reserved2;
		uint16_t reserved3;

		uint16_t iopbOffset;

	} __attribute__((packed));


	constexpr system_segment_t makeTSSEntry(uint64_t base, uint32_t limit)
	{
		constexpr int dpl = 0x3;
		return system_segment_t {
			.limit_low      = (uint16_t) (limit & 0xFFFF),
			.base_low       = (uint16_t) (base & 0xFFFF),
			.base_mid       = (uint8_t)  ((base & 0xFF0000) >> 16),
			.type           = (uint8_t)  (0x80 | (dpl << 5) | 0x9), // 0x80 is present; 0x9 is type (0b1001) -- 64-bit tss available.
			.flags          = (uint8_t)  ((limit & 0xF0000) >> 16 | (1 << 5)),
			.base_high      = (uint8_t)  ((base & 0xFF000000) >> 24),
			.base_highest   = (uint32_t) ((base & 0xFFFFFFFF00000000) >> 32)
		};
	}



	constexpr size_t TSS_SIZE_IN_PAGES = 3;

	krt::pair<addr_t, uint16_t> createTSS()
	{
		auto base = gdt::getGDTAddress().first;
		auto size = gdt::getNextSelectorIndex();

		auto tssdesc = (system_segment_t*) (base + size);

		// to control all 64k possible IO ports, we need 65536 bits / 8 = 8192 bytes of the ioperm bitmaps.
		// we always allocate this for every tss.

		size_t tssSize = sizeof(tss_t) + 0x2000;
		static_assert(sizeof(tss_t) < TSS_SIZE_IN_PAGES * PAGE_SIZE);

		// make sure the scheduler has not started.
		assert(scheduler::getInitPhase() < scheduler::SchedulerInitPhase::SchedulerStarted);
		assert(scheduler::getCurrentProcess() == scheduler::getKernelProcess());

		// ok, since this is the kernel process, we are free to map all tsses in the address space.
		auto tssaddr = vmm::allocate(TSS_SIZE_IN_PAGES, vmm::AddressSpace::Kernel);
		assert(tssaddr);

		// set the tss descriptor in the gdt. this doesn't load it.
		*tssdesc = makeTSSEntry(tssaddr, (uint16_t) tssSize);

		auto selector = (uint16_t) size;
		gdt::incrementSelectorIndex(sizeof(system_segment_t));

		return krt::pair(tssaddr, selector);
	}

	void setRSP0(addr_t tssBase, uint64_t rsp0)
	{
		auto tss = (tss_t*) tssBase;
		tss->rsp0 = rsp0;
	}



	extern "C" void nx_x64_loadtss(uint16_t);

	void loadTSS(uint16_t selector)
	{
		nx_x64_loadtss(selector | 0x3);
	}
}
}
}



















