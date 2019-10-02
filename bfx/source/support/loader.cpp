// loader.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "bfx.h"
#include "elf.h"
#include "string.h"

#include "bootboot.h"
#include "../../kernel/include/misc/addrs.h"

namespace bfx
{
	void loadKernel(uint8_t* input, size_t len, uint64_t* entry_out)
	{
		// assume 64-bit, duh
		auto header = (Elf64_Ehdr*) input;

		println("loading nx kernel");

		// verify our assumptions. we want:
		// 1. and ELF file
		// 2. a 64-bit ELF file
		// 3. an executable ELF file
		// 4. a little-endian ELF file. (for now!)
		// 5. a system V ABI ELF file

		if(header->e_ident[EI_MAG0] != ELFMAG0 || header->e_ident[EI_MAG1] != ELFMAG1
			|| header->e_ident[EI_MAG2] != ELFMAG2 || header->e_ident[EI_MAG3] != ELFMAG3)
		{
			abort("invalid ELF file! (wrong header)");
		}

		if(header->e_ident[EI_CLASS] != ELFCLASS64)
			abort("expected 64-bit ELF file!");

		if(header->e_ident[EI_DATA] != ELFDATA2LSB)
			abort("big-endian ELFs not supported at this time!");

		if(header->e_ident[EI_OSABI] != ELFOSABI_SYSV)
			abort("ELF not using System V ABI!");

		if(header->e_type != ET_EXEC)
			abort("ELF file is not executable!");

		for(uint64_t k = 0; k < header->e_phnum; k++)
		{
			// program headers are contiguous, starting from e_phoff.
			auto progHdr = (Elf64_Phdr*) (input + header->e_phoff + (k * header->e_phentsize));

			// skip null headers and headers that won't exist in the final executable.
			if(progHdr->p_type == PT_NULL || progHdr->p_memsz == 0)
				continue;

			// load by pages, rounded up.
			// note: EFI allocates pages in chunks of 0x1000 aka 4KB. we might use large pages later in the kernel, who knows?
			size_t numPages = (progHdr->p_memsz + 0xFFF) / 0x1000;

			uint64_t buffer = pmm::allocate(numPages);
			vmm::mapKernel(buffer, progHdr->p_vaddr, numPages, 0);

			mmap::addEntry(buffer, numPages, (uint64_t) nx::MemoryType::LoadedKernel);

			memmove((void*) buffer, (input + progHdr->p_offset), progHdr->p_filesz);

			// if we have extra space, set them to zero.
			// but! since we have not changed the virtual mapping yet, we use the physical address
			// (which BOOTBOOT identity maps for us) to do the memset.
			if(progHdr->p_memsz > progHdr->p_filesz)
				memset((void*) (buffer + progHdr->p_filesz), 0, (progHdr->p_memsz - progHdr->p_filesz));
		}

		println("kernel loaded! entry point: %p", header->e_entry);
		*entry_out = header->e_entry;
	}


	nx::BootInfo* setupBootInfo(BOOTBOOT* bbinfo)
	{
		nx::BootInfo* bi = (nx::BootInfo*) pmm::allocate(1);

		mmap::addEntry((uint64_t) bi, 1, (uint64_t) nx::MemoryType::BootInfo);
		vmm::mapKernel((uint64_t) bi, (uint64_t) bi, 1, 0);

		bi->ident[0]    = 'b';
		bi->ident[1]    = 'f';
		bi->ident[2]    = 'x';
		bi->flags       = nx::BOOTINFO_FLAG_BIOS;
		bi->version     = NX_BOOTINFO_VERSION;

		bi->fbHorz      = bbinfo->fb_width;
		bi->fbVert      = bbinfo->fb_height;
		bi->fbScanWidth = bbinfo->fb_scanline / 4;
		bi->frameBuffer = nx::addrs::KERNEL_FRAMEBUFFER;

		bi->pml4Address = vmm::getPML4Address();

		bi->bios.acpiTable = (void*) bbinfo->arch.x86_64.acpi_ptr;

		return bi;
	}
}









