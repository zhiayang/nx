// sc_vfs.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

#include <libc/include/unistd.h>

namespace nx {
namespace syscall
{
	ssize_t sc_vfs_read(int fd, void* buf, size_t len)
	{
		// no, stop!
		return 0;
	}

	ssize_t sc_vfs_write(int fd, const void* buf, size_t len)
	{
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
}
