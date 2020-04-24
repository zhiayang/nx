// sc_vfs.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

// #include <libc/include/unistd.h>

namespace nx
{
	ssize_t syscall::vfs_read(int fd, void* buf, size_t len)
	{
		// no, stop!
		return 0;
	}

	ssize_t syscall::vfs_write(int fd, const void* buf, size_t len)
	{
		// what this needs to do is to go into the current process, look at its vfs table,
		// and call ->write on the appropriate thing!

		if(fd == 1 || fd == 2)
		{
			print("%.*s", len, buf);
			return len;
		}
		else
		{
			// unsupported!!
			return 0;
		}
	}
}
