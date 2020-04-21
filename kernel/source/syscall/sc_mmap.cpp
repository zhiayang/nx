// sc_mmap.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

#include <libc/include/sys/mman.h>

namespace nx
{
	/*
		there's some stuff we need to handle for unix-like mmap. i believe the linux behaviour
		is that MAP_SHARED is only designed to work with files; if you do MAP_ANON | MAP_SHARED,
		the memory is only visible to the child process after a fork...

		i think.
	*/

	void* syscall::mmap_anon(void* req_addr, size_t length, int prot, int flags)
	{
		if(length == 0)
			return MAP_FAILED;

		if(flags & MAP_ANONYMOUS)
		{
			// not supported also!
			if(flags & MAP_SHARED)
				return MAP_FAILED;

			// align the memory
			auto addr = PAGE_ALIGN((addr_t) req_addr);
			auto numPages = (length + PAGE_SIZE - 1) / PAGE_SIZE;

			// TODO: mark non-present when we get swapping and stuff
			uint64_t flg = vmm::PAGE_PRESENT | vmm::PAGE_USER;

			if(prot & PROT_WRITE)   flg |= vmm::PAGE_WRITE;
			if(!(prot & PROT_EXEC)) flg |= vmm::PAGE_NX;

			// TODO: check the bounds of the request!
			if(req_addr != 0)
			{
				// ok, try to allocate some memory first.
				auto virt = vmm::allocateSpecific(VirtAddr(addr), numPages);
				if(virt.nonZero())
				{
					vmm::mapLazy(virt, numPages, flg & ~vmm::PAGE_WRITE);
					return virt.ptr();
				}

				// else: fallthrough, but only if we didn't ask for MAP_FIXED.
				if((flags & MAP_FIXED) || (flags & MAP_FIXED_NOREPLACE))
					return MAP_FAILED;
			}

			auto ret = vmm::allocate(numPages, vmm::AddressSpaceType::User, flg);
			if(ret.isZero()) return MAP_FAILED;

			log("mmap", "user mmap: %p -> %p", ret.addr(), (ret + ofsPages(numPages)).addr());
			return ret.ptr();
		}
		else
		{
			// not supported!
			return MAP_FAILED;
		}
	}


	void* syscall::mmap_file(void* req_addr, size_t length, int prot, int flags, int fd, off_t offset)
	{
		return MAP_FAILED;
	}



	int syscall::munmap(void* req_addr, size_t length)
	{
		// well, let's just unmap it page-by-page.

		auto addr = VirtAddr((addr_t) req_addr).pageAligned();
		auto numPages = (length + PAGE_SIZE - 1) / PAGE_SIZE;

		for(size_t i = 0; i < numPages; i++)
		{
			auto v = addr + ofsPages(i);

			if(vmm::isMapped(v, 1))
			{
				auto phys = vmm::getPhysAddr(v);
				vmm::unmapAddress(v, 1);
				pmm::deallocate(phys, 1);
			}
		}

		return 0;
	}
}























