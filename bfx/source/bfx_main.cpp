// bfx_main.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdint.h>
#include <stddef.h>

#include "bfx.h"
#include "krt.h"

#include "bootboot.h"
#include "../../kernel/include/bootinfo.h"

#define BFX_VERSION_STRING "0.2.4"
#define KERNEL_FILENAME    "boot/nxkernel64"

extern "C" void kernel_trampoline(uint64_t cr3, uint64_t entry, uint64_t bootinfo);
extern "C" void kernel_trampoline_end();

namespace bfx::params { extern size_t NUM_ALLOC_PAGES; }

// note: similar to efx, all memory (at least <4gb) is identity mapped, so we don't need to worry
// about any translation or whatever when passing stuff around. we can just deal in physical addresses.
void bfx::init(BOOTBOOT* bbinfo)
{
	// start the console. 0xFFFFFFFFFC000000 is the fixed address as defined by level1 bootboot loaders.
	// bootboot defines the scanline as the number of bytes, not pixels, to advance. so divide by 4.
	console::init((void*) 0xFFFFFFFFFC000000, bbinfo->fb_width, bbinfo->fb_height, bbinfo->fb_scanline/4);

	println("bfx bootloader");
	println("version %s\n", BFX_VERSION_STRING);

	// just in case.
	idt::init();
	exceptions::init();

	// start the pmm
	{
		MMapEnt* mmapEntries = &bbinfo->mmap;
		size_t numEntries = ((((uint8_t*) bbinfo) + bbinfo->size) - (uint8_t*) mmapEntries) / sizeof(MMapEnt);

		pmm::init(mmapEntries, numEntries);
		mmap::init(mmapEntries, numEntries);
	}


	{
		println("display resolution: %dx%d (scan line %d)", bbinfo->fb_width, bbinfo->fb_height, bbinfo->fb_scanline / 4);
		println("framebuffer: %p, size: %zu bytes (%d pages)", bbinfo->fb_ptr, bbinfo->fb_scanline * bbinfo->fb_height,
			(bbinfo->fb_scanline * bbinfo->fb_height + 0xFFF) / 0x1000);

		println("");
	}


	vmm::setupCR3(bbinfo);
	nx::BootInfo* kbootinfo = setupBootInfo(bbinfo);

	uint64_t kernelEntry = 0;
	{
		// copy the initrd somewhere.
		void* initrdPtr = 0;
		size_t initrdSz = 0;

		{
			size_t numPages = (bbinfo->initrd_size + PAGE_SIZE - 1) / PAGE_SIZE;
			uint64_t ptr = pmm::allocate(numPages);

			vmm::mapKernel(ptr, ptr, numPages, 0);
			mmap::addEntry(ptr, numPages, (uint64_t) nx::MemoryType::Initrd);
			memcpy((void*) ptr, (void*) bbinfo->initrd_ptr, bbinfo->initrd_size);

			initrdPtr = (void*) ptr;
			initrdSz  = bbinfo->initrd_size;

			kbootinfo->initrdBuffer = initrdPtr;
			kbootinfo->initrdSize = initrdSz;
		}

		void* kernelPtr = 0;
		size_t kernelSize = 0;

		{
			auto [ ptr, sz ] = initrd::findFile(initrdPtr, initrdSz, KERNEL_FILENAME);

			if(ptr == 0 || sz == 0)
				abort("failed to load kernel ('%s') from initrd!", KERNEL_FILENAME);

			// but first, load the kernel somewhere sane.
			size_t pgs = (sz + PAGE_SIZE - 1) / PAGE_SIZE;
			uint64_t kptr = pmm::allocate(pgs);
			mmap::addEntry(kptr, pgs, (uint64_t) nx::MemoryType::KernelElf);
			vmm::mapKernel(kptr, kptr, pgs, 0);

			memcpy((void*) kptr, ptr, sz);

			kernelPtr = (void*) kptr;
			kernelSize = sz;
		}

		// load the kernel.
		loadKernel((uint8_t*) kernelPtr, kernelSize, &kernelEntry);

		kbootinfo->kernelElf = kernelPtr;
		kbootinfo->kernelElfSize = kernelSize;

		println("");

		params::readParams(kbootinfo, initrdPtr, initrdSz);
		{
			auto p = (uintptr_t) kbootinfo->kernelParams;
			mmap::addEntry(p, bfx::params::NUM_ALLOC_PAGES, (uint64_t) nx::MemoryType::BootInfo);
			vmm::mapKernel(p, p, bfx::params::NUM_ALLOC_PAGES, 0);
		}
	}

	// finally, setup the memory map.
	mmap::finalise(kbootinfo);
	println("");

	/*
		now, jump to the kernel. BUT, we face a problem:
		bootboot, in all its wisdom, is designed to load a kernel directly -- into the higher half.
		problem: [nx] needs to be in the higher half as well. so, when we switch to the kernel cr3,
		the loader code (us) is no longer mapped. so, we cannot read the 'jmp' instruction.

		solution: setup a trampoline in low memory, mapped by the kernel, that has instructions to
		(1) switch cr3
		(2) jump to kernel
	*/

	// make a new page for the trampoline. we don't really need an entry for it in the mmap,
	// cos once the kernel is called then it becomes useless (ie. free memory).
	{
		uint64_t tramp_addr = pmm::allocate(1);
		vmm::mapKernel(tramp_addr, tramp_addr, 1, 0);

		size_t tramp_size = (uint64_t) &kernel_trampoline_end - (uint64_t) &kernel_trampoline;

		println("trampoline: %p, size: %zu", tramp_addr, tramp_size);

		// trampoline is in support/trampoline.s
		memcpy((void*) tramp_addr, (void*) &kernel_trampoline, tramp_size);

		auto trampoline = (void (*)(uint64_t, uint64_t, uint64_t)) tramp_addr;

		// off we go.
		println("jumping to kernel; good luck!\n\n");

		trampoline(vmm::getPML4Address(), kernelEntry, (uint64_t) kbootinfo);
	}
}

















// handled by the linker script
extern "C" BOOTBOOT bootboot;

// entry point. we are in long mode. we modified BOOTBOOT
// so it doesn't start the APs.
extern "C" void entry()
{
	bfx::println("");

	bfx::init(&bootboot);
	while(true);
}







