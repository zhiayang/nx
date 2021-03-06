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

		bool isUserProc = (proc->flags & scheduler::Process::PROC_USER);

		constexpr const char* err = "loader/elf";

		// note: to simplify the code, we'll always treat it as if we were loading to another proc.
		// this may not be true in exec() scenarios, but who cares. it's a mess now with all the
		// isOtherProc checking. we'll be less efficient since we need to map/unmap stuff, but meh.


		auto header = (Elf64_Ehdr*) file;
		{
			auto checkmagic = [](const char* name, char expected, char found) -> bool {
				if(expected != found)
				{
					error(err, "{} wrong; expected {02x}, found {02x}", name, expected, found);
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

		#if __ARCH_x64__
		{
			if(header->e_machine != EM_X86_64)
			{
				error(err, "invalid architecture (expected x86_64, found {})", header->e_machine);
				return false;
			}
			if(header->e_ident[EI_OSABI] != ELFOSABI_SYSV)
			{
				error(err, "invalid abi (expected system v, found {})", header->e_ident[EI_OSABI]);
				return false;
			}
		}
		#elif __ARCH_AARCH64__
		{
			if(header->e_machine != EM_AARCH64)
			{
				error(err, "invalid architecture (expected aarch64, found {})", header->e_machine);
				return false;
			}
			if(header->e_ident[EI_OSABI] != ELFOSABI_ARM)
			{
				error(err, "invalid abi (expected arm, found {})", header->e_ident[EI_OSABI]);
				return false;
			}
		}
		#else
		{
			error(err, "unsupported architecture {}", header->e_machine);
			return false;
		}
		#endif

		if(header->e_type != ET_EXEC)   { error(err, "non-executables not currently supported"); return false; }


		// ok, we *should* be good to start loading the things.
		// first, load the segments.
		for(uint64_t k = 0; k < header->e_phnum; k++)
		{
			// program headers are contiguous, starting from e_phoff.
			auto progHdr = (Elf64_Phdr*) (file + header->e_phoff + (k * header->e_phentsize));

			// skip null headers and headers that won't exist in the final executable.
			if((progHdr->p_type != PT_LOAD && progHdr->p_type != PT_TLS) || progHdr->p_memsz == 0)
				continue;

			// make sure the things match
			if((progHdr->p_offset % progHdr->p_align) != (progHdr->p_vaddr % progHdr->p_align))
			{
				error(err, "misaligned segment");
				return false;
			}


			// allocate the physical pages.
			size_t numPages = (progHdr->p_memsz + PAGE_SIZE - 1) / PAGE_SIZE;

			auto phys = pmm::allocate(numPages);
			if(phys.isZero()) { error(err, "failed to allocate memory"); return false; }

			proc->addrspace.lock()->addTrackedPhysPages(phys, numPages);

			// figure out how we should map this.
			uint64_t virtFlags = 0;

			if(isUserProc)                                  virtFlags |= vmm::PAGE_USER;
			if(progHdr->p_flags & PF_W)                     virtFlags |= vmm::PAGE_WRITE;
			if(!(progHdr->p_flags & PF_X))                  virtFlags |= vmm::PAGE_NX;

			// gimme some scratch space in the current addrspace
			auto virtBase = VirtAddr::zero();
			size_t offsetVirt = 0;

			// give us some scratch space:
			{
				virtBase = vmm::allocateAddrSpace(numPages, vmm::AddressSpaceType::User);

				// since this is not the final mapping, we don't need user perms.
				vmm::mapAddress(virtBase, phys, numPages, vmm::PAGE_PRESENT | vmm::PAGE_WRITE);

				offsetVirt = (virtBase + (progHdr->p_vaddr - PAGE_ALIGN(progHdr->p_vaddr))).addr();
			}

			assert(offsetVirt);
			memmove((void*) offsetVirt, (file + progHdr->p_offset), progHdr->p_filesz);

			// zero out any extra space.
			memset((void*) (offsetVirt + progHdr->p_filesz), 0, (progHdr->p_memsz - progHdr->p_filesz));


			// ok, we need to unmap the scratch space and do the real mapping.
			{
				vmm::deallocateAddrSpace(virtBase, numPages);
				vmm::unmapAddress(virtBase, numPages);

				if(progHdr->p_type == PT_TLS)
				{
					// for TLS, we don't care what the address is.
					auto virt = vmm::allocateAddrSpace(numPages, vmm::AddressSpaceType::User, proc);
					if(virt.isZero()) { error(err, "failed to allocate address space"); return false; }

					vmm::mapAddress(virt, phys, numPages, virtFlags, proc);

					proc->tlsMasterCopy = (virt + (progHdr->p_vaddr - PAGE_ALIGN(progHdr->p_vaddr))).addr();
					proc->tlsAlign = __max(progHdr->p_align, (uint64_t) 1);
					proc->tlsSize = progHdr->p_memsz;
				}
				else
				{
					if(vmm::allocateSpecific(VirtAddr(progHdr->p_vaddr).pageAligned(), numPages, proc).isZero())
					{
						error(err, "could not allocate address {p} in the target address space", progHdr->p_vaddr);

						pmm::deallocate(phys, numPages);
						proc->addrspace.lock()->removeTrackedPhysPages(phys, numPages);

						return false;
					}

					vmm::mapAddress(VirtAddr(progHdr->p_vaddr).pageAligned(), phys, numPages, virtFlags, proc);
				}
			}
		}

		assert(entry);
		*entry = header->e_entry;

		return true;
	}

















}
}



























