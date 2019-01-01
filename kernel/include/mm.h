// mm.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include "defs.h"

namespace nx
{
	struct BootInfo;

	namespace pmm
	{
		void init(BootInfo* bootinfo);
		addr_t allocate(size_t num, bool below4G = false);
	}

	namespace vmm
	{
		struct pml_t
		{
			uint64_t entries[512];
		};

		void invalidate(addr_t addr);
		void mapAddress(addr_t virt, addr_t phys, size_t num, uint64_t flags);


		static constexpr size_t indexPML4(addr_t addr)       { return ((((addr_t) addr) >> 39) & 0x1FF); }
		static constexpr size_t indexPDPT(addr_t addr)       { return ((((addr_t) addr) >> 30) & 0x1FF); }
		static constexpr size_t indexPageDir(addr_t addr)    { return ((((addr_t) addr) >> 21) & 0x1FF); }
		static constexpr size_t indexPageTable(addr_t addr)  { return ((((addr_t) addr) >> 12) & 0x1FF); }

		static constexpr addr_t RecursiveAddrs[] = {
			0xFFFF'FF00'0000'0000ULL + (0x4000'0000ULL * 510) + (0x20'0000ULL * 510) + (0x1000 * 510),
			0xFFFF'FF00'0000'0000ULL + (0x4000'0000ULL * 510) + (0x20'0000ULL * 510),
			0xFFFF'FF00'0000'0000ULL + (0x4000'0000ULL * 510),
			0xFFFF'FF00'0000'0000ULL
		};

		constexpr uint64_t PAGE_PRESENT     = 0x1;
		constexpr uint64_t PAGE_WRITE       = 0x2;
		constexpr uint64_t PAGE_USER        = 0x4;

		constexpr uint64_t PAGE_NO_FLAGS    = ~0xFFF;
	}
}












