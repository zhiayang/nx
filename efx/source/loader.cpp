// loader.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "efx.h"
#include "krt.h"
#include "efi/system-table.h"

#include "elf.h"

namespace efx
{
	efx::array<KernelVirtMapping> loadKernel(uint8_t* input, size_t len, uint64_t* entry_out)
	{
		// assume 64-bit, duh
		auto header = (Elf64_Ehdr*) input;

		efi::println("loading nx kernel");

		// verify our assumptions. we want:
		// 1. and ELF file
		// 2. a 64-bit ELF file
		// 3. an executable ELF file
		// 4. a little-endian ELF file. (for now!)
		// 5. a system V ABI ELF file

		if(header->e_ident[EI_MAG0] != ELFMAG0 || header->e_ident[EI_MAG1] != ELFMAG1
			|| header->e_ident[EI_MAG2] != ELFMAG2 || header->e_ident[EI_MAG3] != ELFMAG3)
		{
			efi::abort("invalid ELF file! (header wrong)");
		}

		if(header->e_ident[EI_CLASS] != ELFCLASS64)
			efi::abort("expected 64-bit ELF file!");

		if(header->e_ident[EI_DATA] != ELFDATA2LSB)
			efi::abort("big-endian ELFs not supported at this time!");

		if(header->e_ident[EI_OSABI] != ELFOSABI_SYSV)
			efi::abort("ELF not using System V ABI!");

		if(header->e_type != ET_EXEC)
			efi::abort("ELF file is not executable!");

		efx::array<KernelVirtMapping> virtMappings;
		for(uint64_t k = 0; k < header->e_phnum; k++)
		{
			// program headers are contiguous, starting from e_phoff.
			auto progHdr = (Elf64_Phdr*) (input + header->e_phoff + (k * header->e_phentsize));

			// skip null headers and headers that won't exist in the final executable.
			if(progHdr->p_type == PT_NULL || progHdr->p_memsz == 0)
				continue;

			// load by pages, rounded up.
			// note: EFI allocates pages in chunks of 0x1000 aka 4KB. we might use large pages later in the kernel, who knows?
			size_t numPages = (progHdr->p_memsz + 0x1000) / 0x1000;

			uint64_t buffer = 0;
			auto stat = efi::systable()->BootServices->AllocatePages(AllocateAnyPages, EfiLoaderCode, numPages, &buffer);
			efi::abort_if_error(stat, "loadKernel(): failed to allocate memory for kernel (wanted %zu pages)", numPages);

			// we need to mark these as 'used', so the kernel knows about them -- and doesn't mistakenly hand these
			// pages out and overwrite itself :D
			// note that we only do this when we allocate pages -- the EFI heap just contains junk that we can discard.
			// notably, the bootinfo struct will also need to be marked, as well as the initial CR3.
			memory::markPhyiscalPagesUsed(buffer, numPages);

			KernelVirtMapping mp;
			mp.virt = progHdr->p_vaddr;
			mp.phys = buffer;
			mp.num = numPages;

			if(progHdr->p_flags & PF_R) mp.read = true;
			if(progHdr->p_flags & PF_W) mp.write = true;
			if(progHdr->p_flags & PF_X) mp.execute = true;

			virtMappings.append(mp);
			memory::mapVirtual(mp.phys, mp.virt, mp.num);

			memmove((void*) buffer, (input + progHdr->p_offset), progHdr->p_filesz);

			// if we have extra space, set them to zero.
			// but! since we have not changed the virtual mapping yet, we use the physical address
			// (which UEFI identity maps for us) to do the memset.
			if(progHdr->p_memsz > progHdr->p_filesz)
				memset((void*) (buffer + progHdr->p_filesz), 0, (progHdr->p_memsz - progHdr->p_filesz));
		}

		efi::println("kernel loaded! entry point: %p", header->e_entry);

		*entry_out = header->e_entry;
		return virtMappings;
	}
}

















