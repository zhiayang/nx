// syscall.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <sys/types.h>
#include <nx/syscall_funcs.h>

#if defined(__x86_64__)

	#include <nx/arch/x86_64/syscall_macros.h>

#else

	#error "unsupported architecture!"

#endif

#ifdef __cplusplus
namespace syscall
{
	void exit(int status);

	void* mmap_anon(void* addr, size_t length, int prot, int flags);
	void* mmap_file(void* addr, size_t length, int prot, int flags, int fd, off_t offset);
	int munmap(void* addr, size_t length);

	ssize_t vfs_read(int fd, void* buf, size_t len);
	ssize_t vfs_write(int fd, const void* buf, size_t len);

	void kernel_log(int lvl, const char* sys, size_t slen, const char* str, size_t len);

	uint64_t nanosecond_timestamp();
}
#else
	#error unsupported
#endif











