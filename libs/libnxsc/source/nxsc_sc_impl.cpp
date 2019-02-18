// nxsc_sc_impl.cpp
// Copyright (c) 2014 - 2016, zhiayang
// Licensed under the Apache License Version 2.0.

#include <errno.h>
#include <syscall.h>

namespace syscall
{
	using namespace nx;

	void exit(int status)
	{
		__nx_syscall_0v(SYSCALL_EXIT);
	}

	void* mmap_anon(void* addr, size_t length, int prot, int flags)
	{
		void* ret = 0;
		__nx_syscall_4(SYSCALL_MMAP_ANON, ret, addr, length, prot, flags);

		return ret;
	}

	void* mmap_file(void* addr, size_t length, int prot, int flags, int fd, off_t offset)
	{
		void* ret = 0;
		__nx_syscall_6(SYSCALL_MMAP_FILE, ret, addr, length, prot, flags, fd, offset);

		return ret;
	}

	int munmap(void* addr, size_t length)
	{
		return -1;
	}

	ssize_t vfs_read(int fd, void* buf, size_t len)
	{
		ssize_t ret = 0;
		__nx_syscall_3(SYSCALL_VFS_READ, ret, fd, buf, len);
		return ret;
	}

	ssize_t vfs_write(int fd, const void* buf, size_t len)
	{
		ssize_t ret = 0;
		__nx_syscall_3(SYSCALL_VFS_WRITE, ret, fd, buf, len);
		return ret;
	}
}






extern "C" void* __fetch_errno()
{
	return &errno;
}














