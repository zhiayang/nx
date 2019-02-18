// mmap.cpp
// Copyright (c) 2014 - 2016, zhiayang
// Licensed under the Apache License Version 2.0.


#include <stdlib.h>
#include <sys/mman.h>

#include <syscall.h>

extern "C" void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset)
{
	if(flags & MAP_ANONYMOUS)
	{
		return syscall::mmap_anon(addr, length, prot, flags);
	}
	else
	{
		if(fd > 0 || offset > 0)
		{
			abort();
		}

		abort();
	}

	return MAP_FAILED;
}

extern "C" int munmap(void* addr, size_t length)
{
	(void) addr;
	(void) length;
	return 0;
}



