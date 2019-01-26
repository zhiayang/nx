// bootinfo.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"

namespace nx
{
	enum class MemoryType : uint64_t
	{
		Reserved,
		Available,
		ACPI,
		MMIO,
		Faulty,
		NonVolatile,
		Framebuffer,

		LoadedKernel,
		BootInfo,
		MemoryMap,
		Initrd,
		KernelElf,

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
		uint8_t version;

		uint32_t fbHorz;        // horizontal resolution (in pixels)
		uint32_t fbVert;        // vertical res
		uint32_t fbScanWidth;   // 'actual width' -- number of pixels to go to the next vertical line.
		uint64_t frameBuffer;   // pyhs address of the frame buffer

		uint64_t pml4Address;   // physical address of the PML4T that was set up by the bootloader.

		void* efiSysTable;      // pointer to EFI system table
		bool canCallEFIRuntime; // whether or not we successfully called SetVirtualMemoryMap()

		uint64_t mmEntryCount;
		MemMapEntry* mmEntries;

		uint64_t initrdSize;    // we only support one initrd right now!
		void* initrdBuffer;     // physical address, but the loader will identity map it (since it is LoaderSetup memory)


		// from version 2:
		void* kernelElf;        // the entire kernel executable
		uint64_t kernelElfSize; // how large the thing is.
	};
}









