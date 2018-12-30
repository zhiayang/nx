// kernel.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"

namespace nx
{
	namespace consts
	{
		constexpr uintptr_t KERNEL_ENTRY        = 0xFFFFFFFF'80000000;
		constexpr uintptr_t KERNEL_CR3_ADDRESS  = 0xFFFFFFFF'D0000000;
	}

	namespace vmm
	{
		struct pagetable_t  { uint64_t      entries[512]; };
		struct pagedir_t    { pagetable_t*  entries[512]; };
		struct pdpt_t       { pagedir_t*    entries[512]; };
		struct pml4t_t      { pdpt_t*       entries[512]; };

		static constexpr size_t indexPML4(uintptr_t addr)       { return ((((uintptr_t) addr) >> 39) & 0x1FF); }
		static constexpr size_t indexPDPT(uintptr_t addr)       { return ((((uintptr_t) addr) >> 30) & 0x1FF); }
		static constexpr size_t indexPageDir(uintptr_t addr)    { return ((((uintptr_t) addr) >> 21) & 0x1FF); }
		static constexpr size_t indexPageTable(uintptr_t addr)  { return ((((uintptr_t) addr) >> 12) & 0x1FF); }

		constexpr uint64_t PAGE_PRESENT     = 0x1;
		constexpr uint64_t PAGE_WRITE       = 0x2;
		constexpr uint64_t PAGE_USER        = 0x4;
	}
}