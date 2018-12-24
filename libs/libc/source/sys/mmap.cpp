// mmap.cpp
// Copyright (c) 2014 - 2016, zhiayang
// Licensed under the Apache License Version 2.0.


#include "../../include/sys/mman.h"
#include "../../include/assert.h"
#include "../../include/stdlib.h"
#include "../../include/orionx/syscall.h"


extern "C" void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset)
{
	if(flags & MAP_ANONYMOUS)
	{
		return (void*) Library::SystemCall::MMap_Anonymous((uint64_t) addr, length, prot, flags);
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



