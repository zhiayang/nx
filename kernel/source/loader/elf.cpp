// elf.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "loader.h"

#include "elf.h"

namespace nx {
namespace loader
{
	bool loadELFBinary(scheduler::Process* proc, void* buf, size_t len, addr_t* entry)
	{
		assert(buf);
		assert(proc);
		assert(len > 0);

		auto file = (uint8_t*) buf;

		bool isOtherProc = (proc != scheduler::getCurrentProcess());
		bool isUserProc = (proc->flags & scheduler::Process::PROC_USER);

		constexpr const char* err = "loader/elf";



		auto header = (Elf64_Ehdr*) file;
		{
			auto checkmagic = [](const char* name, char expected, char found) -> bool {
				if(expected != found)
				{
					error(err, "%s wrong; expected %02x, found %02x", name, expected, found);
					return false;
				}
				return true;
			};

			if(!checkmagic("ELFMAG0", ELFMAG0, header->e_ident[EI_MAG0])) return false;
			if(!checkmagic("ELFMAG1", ELFMAG1, header->e_ident[EI_MAG1])) return false;
			if(!checkmagic("ELFMAG2", ELFMAG2, header->e_ident[EI_MAG2])) return false;
			if(!checkmagic("ELFMAG3", ELFMAG3, header->e_ident[EI_MAG3])) return false;
		}


		if(header->e_ident[EI_CLASS] != ELFCLASS64) { error(err, "32-bit executables not supported"); return false; }
		if(header->e_ident[EI_DATA] != ELFDATA2LSB) { error(err, "big-endian executables not supported"); return false; }

		if constexpr (getArchitecture() == Architecture::x64)
		{
			if(header->e_machine != EM_X86_64)
			{
				error(err, "invalid architecture (expected x86_64, found %d)", header->e_machine);
				return false;
			}
			if(header->e_ident[EI_OSABI] != ELFOSABI_SYSV)
			{
				error(err, "invalid abi (expected system v, found %d)", header->e_ident[EI_OSABI]);
				return false;
			}
		}
		else if constexpr (getArchitecture() == Architecture::AArch64)
		{
			if(header->e_machine != EM_AARCH64)
			{
				error(err, "invalid architecture (expected aarch64, found %d)", header->e_machine);
				return false;
			}
			if(header->e_ident[EI_OSABI] != ELFOSABI_ARM)
			{
				error(err, "invalid abi (expected arm, found %d)", header->e_ident[EI_OSABI]);
				return false;
			}
		}
		else
		{
			error(err, "unsupported architecture %d", header->e_machine);
			return false;
		}

		if(header->e_type != ET_EXEC)   { error(err, "non-executables not currently supported"); return false; }


		// ok, we *should* be good to start loading the things.
		// first, load the segments.
		for(uint64_t k = 0; k < header->e_phnum; k++)
		{
			// program headers are contiguous, starting from e_phoff.
			auto progHdr = (Elf64_Phdr*) (file + header->e_phoff + (k * header->e_phentsize));

			// skip null headers and headers that won't exist in the final executable.
			if(progHdr->p_type != PT_LOAD || progHdr->p_memsz == 0)
				continue;

			// make sure the things match
			if((progHdr->p_offset % progHdr->p_align) != (progHdr->p_vaddr % progHdr->p_align))
			{
				error(err, "misaligned segment");
				return false;
			}


			// allocate the physical pages.
			size_t numPages = (progHdr->p_memsz + 0x1000) / 0x1000;

			auto phys = pmm::allocate(numPages);
			if(!phys) { error(err, "failed to allocate memory"); return false; }

			// figure out how we should map this.
			uint64_t virtFlags = 0;

			if(isUserProc)                                  virtFlags |= vmm::PAGE_USER;
			if(progHdr->p_flags & PF_W)                     virtFlags |= vmm::PAGE_WRITE;
			if(!(progHdr->p_flags & PF_X))                  virtFlags |= vmm::PAGE_NX;

			// gimme some scratch space in the current addrspace
			addr_t virtBase = 0;
			addr_t offsetVirt = 0;

			if(isOtherProc)
			{
				virtBase = vmm::allocateAddrSpace(numPages, vmm::AddressSpace::User);

				// since this is not the final mapping, we don't need user perms.
				vmm::mapAddress(virtBase, phys, numPages, vmm::PAGE_PRESENT | vmm::PAGE_WRITE);

				offsetVirt = virtBase + (progHdr->p_vaddr - (progHdr->p_vaddr & vmm::PAGE_ALIGN));
			}
			else
			{
				virtBase = vmm::allocateSpecific(progHdr->p_vaddr & vmm::PAGE_ALIGN, numPages, proc);
				if(!virtBase)
				{
					error(err, "could not allocate address %p in the target address space", progHdr->p_vaddr);
					pmm::deallocate(phys, numPages);
					return false;
				}

				// this is the final mapping, so we need the proper flags.
				vmm::mapAddress(virtBase, phys, numPages, virtFlags);

				offsetVirt = virtBase + (progHdr->p_vaddr - (progHdr->p_vaddr & vmm::PAGE_ALIGN));
			}

			assert(offsetVirt);
			memmove((void*) offsetVirt, (file + progHdr->p_offset), progHdr->p_filesz);

			// zero out any extra space.
			memset((void*) (offsetVirt + progHdr->p_filesz), 0, (progHdr->p_memsz - progHdr->p_filesz));


			// ok, if it is not the current address space, we need to unmap the scratch space and do the real mapping.
			if(isOtherProc)
			{
				vmm::deallocateAddrSpace(virtBase, numPages);
				vmm::unmapAddress(virtBase, numPages, /* freePhys: */ false);

				if(vmm::allocateSpecific(progHdr->p_vaddr & vmm::PAGE_ALIGN, numPages, proc) == 0)
				{
					error(err, "could not allocate address %p in the target address space", progHdr->p_vaddr);
					pmm::deallocate(phys, numPages);
					return false;
				}

				vmm::mapAddress(progHdr->p_vaddr & vmm::PAGE_ALIGN, phys, numPages, virtFlags | 0x4, proc);
			}
		}

		assert(entry);
		*entry = header->e_entry;

		return true;
	}

















	// note: we put it here cos for now elfs (elves?) are the only thing we support
	bool loadIndeterminateBinary(scheduler::Process* proc, void* buf, size_t len, addr_t* entry)
	{
		assert(buf);
		assert(proc);
		assert(len > 0);

		// check for ELF
		{
			auto tmp = (char*) buf;
			if(tmp[0] == ELFMAG0 && tmp[1] == ELFMAG1 && tmp[2] == ELFMAG2 && tmp[3] == ELFMAG3)
				return loadELFBinary(proc, buf, len, entry);
		}

		// oopss.
		error("loader", "unable to determine executable format; loading failed");
		return false;
	}
}
}



























