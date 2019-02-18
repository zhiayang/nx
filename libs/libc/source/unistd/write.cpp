// write.cpp
// Copyright (c) 2014 - 2016, zhiayang
// Licensed under the Apache License Version 2.0.

#include <unistd.h>
#include <syscall.h>

extern "C" ssize_t write(int fd, const void* buf, size_t len)
{
	return syscall::vfs_write(fd, buf, len);
}
