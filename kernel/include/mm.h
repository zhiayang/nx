// mm.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"

namespace nx
{
	struct MemMapEntry;

	namespace pmm
	{
		void init(MemMapEntry* mmEntries, size_t numEntries);
	}

	namespace vmm
	{
		struct pml_t
		{
			uint64_t entries[512];
		};

		static constexpr size_t indexPML4(uintptr_t addr)       { return ((((uintptr_t) addr) >> 39) & 0x1FF); }
		static constexpr size_t indexPDPT(uintptr_t addr)       { return ((((uintptr_t) addr) >> 30) & 0x1FF); }
		static constexpr size_t indexPageDir(uintptr_t addr)    { return ((((uintptr_t) addr) >> 21) & 0x1FF); }
		static constexpr size_t indexPageTable(uintptr_t addr)  { return ((((uintptr_t) addr) >> 12) & 0x1FF); }

		constexpr uint64_t PAGE_PRESENT     = 0x1;
		constexpr uint64_t PAGE_WRITE       = 0x2;
		constexpr uint64_t PAGE_USER        = 0x4;

		constexpr uint64_t PAGE_NO_FLAGS    = ~0xFFF;
	}
}












