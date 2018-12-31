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
		constexpr uintptr_t KERNEL_ENTRY                = 0xFFFFFFFF'80000000;
		constexpr uintptr_t EFI_RUNTIME_SERVICES_BASE   = 0xFFFFFFFF'C0000000;
		constexpr uintptr_t KERNEL_FRAMEBUFFER_ADDRESS  = 0xFFFFFFFF'D0000000;
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

	enum class MemoryType : uint64_t
	{
		Reserved,
		Available,
		ACPI,
		MMIO,
		Faulty,
		NonVolatile,
		LoaderSetup,
		Framebuffer,

		EFIRuntimeCode,
		EFIRuntimeData,
	};

	struct MemMapEntry
	{
		uint64_t address;
		uint64_t numPages;
		MemoryType memoryType;
		uint64_t efiAttributes;
	};

	struct BootInfo
	{
		uint8_t ident[3];       // E, F, X
		uint8_t version;        // 1

		uint32_t fbHorz;        // horizontal resolution (in pixels)
		uint32_t fbVert;        // vertical res
		uint32_t fbScanWidth;   // 'actual width' -- number of pixels to go to the next vertical line.
		uint64_t frameBuffer;   // pyhs address of the frame buffer

		void* efiSysTable;      // pointer to EFI system table
		bool canCallEFIRuntime; // whether or not we successfully called SetVirtualMemoryMap()

		uint64_t mmEntryCount;
		MemMapEntry* mmEntries;
	};
}













