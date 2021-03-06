// loader.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "efx.h"
#include "krt.h"
#include "efi/system-table.h"

#include "elf.h"
#include "../../kernel/include/misc/addrs.h"

namespace efx
{
	void loadKernel(uint8_t* input, size_t len, uint64_t* entry_out)
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
			efi::abort("invalid ELF file! (wrong header)");
		}

		if(header->e_ident[EI_CLASS] != ELFCLASS64)
			efi::abort("expected 64-bit ELF file!");

		if(header->e_ident[EI_DATA] != ELFDATA2LSB)
			efi::abort("big-endian ELFs not supported at this time!");

		if(header->e_ident[EI_OSABI] != ELFOSABI_SYSV)
			efi::abort("ELF not using System V ABI!");

		if(header->e_type != ET_EXEC)
			efi::abort("ELF file is not executable!");

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

			auto buffer = efi::allocPages(numPages, efi::MemoryType_LoadedKernel, "kernel");

			memory::mapVirtual((uint64_t) buffer, progHdr->p_vaddr, numPages);

			memmove(buffer, (input + progHdr->p_offset), progHdr->p_filesz);

			// if we have extra space, set them to zero.
			// but! since we have not changed the virtual mapping yet, we use the physical address
			// (which UEFI identity maps for us) to do the memset.
			if(progHdr->p_memsz > progHdr->p_filesz)
				memset((void*) ((uint64_t) buffer + progHdr->p_filesz), 0, (progHdr->p_memsz - progHdr->p_filesz));
		}

		efi::println("kernel loaded! entry point: %p\n", header->e_entry);

		*entry_out = header->e_entry;
	}


	nx::BootInfo* prepareKernelBootInfo()
	{
		auto bs = efi::systable()->BootServices;

		// first, allocate 1 page for the boot info. we don't use pool cos we want to tell the OS that this memory is used.
		auto bi = (nx::BootInfo*) efi::allocPages(1, efi::MemoryType_BootInfo, "bootinfo");

		// then, fill in some stuff.
		bi->ident[0]        = 'e';
		bi->ident[1]        = 'f';
		bi->ident[2]        = 'x';
		bi->version         = NX_BOOTINFO_VERSION;
		bi->flags           = nx::BOOTINFO_FLAG_UEFI;
		bi->efi.efiSysTable = (void*) efi::systable();

		bi->fbHorz          = graphics::getX();
		bi->fbVert          = graphics::getY();
		bi->fbScanWidth     = graphics::getPixelsPerScanline();
		bi->frameBuffer     = nx::addrs::KERNEL_FRAMEBUFFER.addr();

		bi->pml4Address     = memory::getPML4Address();

		return bi;
	}

	void setKernelMemoryMap(nx::BootInfo* bi)
	{
		auto bs = efi::systable()->BootServices;

		bi->mmEntryCount = 0;

		size_t descSz = 0;
		size_t key = 0;
		size_t totalBufSz = 0;
		size_t numEntries = 0;
		uint8_t* buffer = 0;
		{
			// get the memory map. because UEFI is dumb, we can't call getmemorymap with NULL to find out how large the thing
			// would be, unlike with some POSIX functions. so we just give it a stupid 1 byte buffer and get the needed size.

			size_t bufSz = 1;
			uint32_t descVer = 0;

			{
				uint8_t buf = 0;

				// this is a really poorly designed function interface.
				auto stat = bs->GetMemoryMap(&bufSz, (efi_memory_descriptor*) &buf, &key, &descSz, &descVer);
				if(stat != EFI_BUFFER_TOO_SMALL) efi::abort_if_error(stat, "prepareKernelBootInfo(): failed to get memory map (1)");
			}


			// give me a 256 byte extra for the size, *just in case*.
			buffer = (uint8_t*) efi::alloc(256 + bufSz);
			bufSz += 256;

			totalBufSz = bufSz;

			// why the fuck do i need to pass pointers to these fucking things?? i don't care about their size or version,
			// that's what sizeof is for??? ugh
			auto stat = bs->GetMemoryMap(&bufSz, (efi_memory_descriptor*) buffer, &key, &descSz, &descVer);
			efi::abort_if_error(stat, "prepareKernelBootInfo(): failed to get memory map (2)");

			numEntries = bufSz / descSz;
		}

		auto [ customExtents, numCustomExtents ] = memory::getCustomExtents();

		size_t neededEntries = 0;
		for(size_t i = 0; i < numEntries; i++)
		{
			// count the number of entries we need to forward to the kernel.
			auto entry = (efi_memory_descriptor*) (buffer + (i * descSz));

			if(krt::match(entry->Type, EfiLoaderCode, EfiLoaderData, EfiConventionalMemory, EfiBootServicesCode, EfiBootServicesData,
				EfiACPIMemoryNVS, EfiACPIReclaimMemory, EfiMemoryMappedIO, EfiMemoryMappedIOPortSpace, EfiPersistentMemory,
				EfiRuntimeServicesCode, EfiRuntimeServicesData, efi::MemoryType_LoadedKernel, efi::MemoryType_BootInfo,
				efi::MemoryType_MemoryMap, efi::MemoryType_VMFrame, efi::MemoryType_Initrd, efi::MemoryType_KernelElf))
			{
				neededEntries++;
			}
		}

		neededEntries += numCustomExtents;

		// ok now, allocate and start to fill in.
		nx::MemMapEntry* entries = 0;
		{
			size_t numPages = 1 + ((neededEntries * sizeof(nx::MemMapEntry)) + 0xFFF) / 0x1000;
			entries = (nx::MemMapEntry*) efi::allocPages(numPages, efi::MemoryType_MemoryMap, "memory map");

			memset(entries, 0, 0x1000 * numPages);
		}


		// get the memory map one last fucking time, because EFI is fucking stupid!!!!!!
		{
			size_t descSz = 0;
			uint32_t descVer = 0;

			auto stat = bs->GetMemoryMap(&totalBufSz, (efi_memory_descriptor*) buffer, &key, &descSz, &descVer);
			efi::abort_if_error(stat, "prepareKernelBootInfo(): failed to get memory map (3)");

			numEntries = totalBufSz / descSz;
		}


		// finally, start adding things.
		for(size_t i = 0; i < numEntries; i++)
		{
			bool skip = false;
			auto efiEnt = (efi_memory_descriptor*) (buffer + (i * descSz));

			auto k = bi->mmEntryCount;

			switch(efiEnt->Type)
			{
				case EfiBootServicesCode:           // fallthrough
				case EfiBootServicesData:           entries[k].memoryType = nx::MemoryType::Reserved; break;

				case EfiRuntimeServicesCode:        entries[k].memoryType = nx::MemoryType::EFIRuntimeCode; break;
				case EfiRuntimeServicesData:        entries[k].memoryType = nx::MemoryType::EFIRuntimeData; break;

				case EfiConventionalMemory:         entries[k].memoryType = nx::MemoryType::Available; break;

				case EfiACPIMemoryNVS:              // fallthrough
				case EfiACPIReclaimMemory:          entries[k].memoryType = nx::MemoryType::ACPI; break;

				case EfiMemoryMappedIO:             // fallthrough
				case EfiMemoryMappedIOPortSpace:    entries[k].memoryType = nx::MemoryType::MMIO; break;

				case EfiPersistentMemory:           entries[k].memoryType = nx::MemoryType::NonVolatile; break;

				case efi::MemoryType_VMFrame:       // fallthrough
				case efi::MemoryType_LoadedKernel:  entries[k].memoryType = nx::MemoryType::LoadedKernel; break;
				case efi::MemoryType_MemoryMap:     entries[k].memoryType = nx::MemoryType::MemoryMap; break;
				case efi::MemoryType_BootInfo:      entries[k].memoryType = nx::MemoryType::BootInfo; break;
				case efi::MemoryType_Initrd:        entries[k].memoryType = nx::MemoryType::Initrd; break;
				case efi::MemoryType_KernelElf:     entries[k].memoryType = nx::MemoryType::KernelElf; break;

				case EfiLoaderData:
				case EfiLoaderCode: {

					// ok, we need to handle this specially, because our custom extents are lumped under loader data.
					// so we need to loop through each of our custom extents and check for an overlap.
					// look at this O(n^2) algorithm fly!

					// we could do this more elegantly, eg. by splitting the segments, but frankly i can't be bothered.
					// if we encounter a region in the efi map that overlaps our custom extent, i'm just gonna throw out the whole block.
					for(size_t i = 0; i < numCustomExtents; i++)
					{
						if(customExtents[i].base >= efiEnt->PhysicalStart
							&& (customExtents[i].base + 0x1000 * customExtents[i].num) <= (efiEnt->PhysicalStart + 0x1000 * efiEnt->NumberOfPages))
						{
							skip = true;
							// flag = 1 means we processed it already
							if(!customExtents[i].flag)
							{
								customExtents[i].flag = 1;

								entries[k].address = customExtents[i].base;
								entries[k].numPages = customExtents[i].num;
								entries[k].efiAttributes = efiEnt->Attribute;
								entries[k].memoryType = (nx::MemoryType) customExtents[i].type;

								bi->mmEntryCount += 1;
								k += 1;
							}
						}
					}

					if(!skip)
						entries[k].memoryType = nx::MemoryType::AvailableAfterUnmap;

				} break;

				// do not include
				default:
					skip = true;
					break;
			}

			if(skip)
				continue;

			entries[k].address = efiEnt->PhysicalStart;
			entries[k].numPages = efiEnt->NumberOfPages;
			entries[k].efiAttributes = efiEnt->Attribute;

			bi->mmEntryCount += 1;
		}

		// flush the rest of the custom extents that we haven't touched.
		for(size_t i = 0; i < numCustomExtents; i++)
		{
			auto k = bi->mmEntryCount;
			if(!customExtents[i].flag)
			{
				customExtents[i].flag = 1;

				entries[k].address = customExtents[i].base;
				entries[k].numPages = customExtents[i].num;
				entries[k].memoryType = (nx::MemoryType) customExtents[i].type;
				entries[k].efiAttributes = 0;

				bi->mmEntryCount += 1;
			}
		}

		// finally, the framebuffer.
		entries[bi->mmEntryCount].address = graphics::getFramebufferAddress();
		entries[bi->mmEntryCount].numPages = (graphics::getFramebufferSize() + 0xFFF) / 0x1000;
		entries[bi->mmEntryCount].memoryType = nx::MemoryType::Framebuffer;
		bi->mmEntryCount++;

		efi::println("created kernel memory map, with %zu entries", bi->mmEntryCount);
		bi->mmEntries = entries;
	}
}




































