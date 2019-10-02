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



	#ifndef NX_BOOTINFO_VERSION
		#error "define NX_BOOTINFO_VERSION before including bootinfo.h!"
	#endif

	constexpr uint64_t BOOTINFO_FLAG_UEFI = 0x1;
	constexpr uint64_t BOOTINFO_FLAG_BIOS = 0x2;


	// unless specifically stated, all pointers point to virtual addresses, which are guaranteed
	// to be mapped in the startup address space.
	struct BootInfo
	{
		// 'e', 'f', 'x'
		uint8_t ident[3];

		// the version of the bootinfo struct.
		uint8_t version;

		// horizontal resolution (in pixels)
		uint32_t fbHorz;

		// vertical resolution (in pixels)
		uint32_t fbVert;

		// 'actual width' -- number of pixels to go to the next vertical line.
		uint32_t fbScanWidth;

		// physical address of the frame buffer
		uint64_t frameBuffer;

		// physical address of the PML4T that was set up by the bootloader.
		uint64_t pml4Address;

		// flags. right now just indicates what firmware we booted from.
		uint64_t flags;

		// the number of memory map entries
		uint64_t mmEntryCount;

		// pointer to the first memory map entry (identity mapped)
		MemMapEntry* mmEntries;

		// the size of the initrd
		uint64_t initrdSize;

		// pointer to the initrd (identity mapped)
		void* initrdBuffer;

		struct {

			// pointer to EFI system table
			void* efiSysTable;

			// whether or not we successfully called SetVirtualMemoryMap()
			bool canCallEFIRuntime;

		} efi;

		struct {

			// pointer to either the RSDT or the XSDT. note: not the pointer, the table itself!
			// no extra information (like which one it is), so we must check the signature before using.
			void* acpiTable;

		} bios;


		#if NX_BOOTINFO_VERSION >= 2

			// pointer to the kernel ELF executable
			void* kernelElf;

			// the size of the kernel executable
			uint64_t kernelElfSize;

		#endif


		#if NX_BOOTINFO_VERSION >= 3

			// a list of kernel parameters; each parameter is null-terminated, and immediately proceeded by the next
			// parameter (if any).
			char* kernelParams;

			// the total size of the parameter string
			uint64_t kernelParamsLength;

			// the number of null-terminated parameters
			uint64_t numKernelParams;

		#endif
	};
}























