// syscall.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once
#include "defs.h"
#include "export/syscall_funcs.h"

namespace nx
{
	namespace syscall
	{
		void init();

		void sc_null();

		void sc_exit(int status);
		int64_t sc_ipc_send(uint64_t target, void* msg, size_t len);
		size_t sc_ipc_peek(void* msg, size_t len);
		size_t sc_ipc_poll();
		void sc_ipc_discard();
		size_t sc_ipc_receive(void* msg, size_t len);

		void* sc_mmap_anon(void* req_addr, size_t length, int prot, int flags);
		void* sc_mmap_file(void* req_addr, size_t length, int prot, int flags, int fd, off_t offset);

		int sc_munmap(void* req_addr, size_t length);

		ssize_t sc_vfs_read(int fd, void* buf, size_t len);
		ssize_t sc_vfs_write(int fd, const void* buf, size_t len);


		void sc_log(int level, char* sys, size_t syslen, char* buf, size_t buflen);
	}
}
