// read.cpp
// Copyright (c) 2014 - 2016, zhiayang
// Licensed under the Apache License Version 2.0.

#include <unistd.h>
#include <nx/syscall.h>

extern "C" ssize_t read(int fd, void* buf, size_t len)
{
	return syscall::vfs_read(fd, buf, len);
}

