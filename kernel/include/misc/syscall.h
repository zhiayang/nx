// syscall.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once
#include "defs.h"
#include "export/ipc_message.h"
#include "export/syscall_funcs.h"

namespace nx
{
	namespace syscall
	{
		void init();

		bool copy_from_user(const void* user, void* kernel, size_t len);
		bool copy_to_user(const void* kernel, void* user, size_t len);

		void sc_get_memory_ticket(ipc::mem_ticket_t* ticket, size_t len);
		void sc_collect_memory(ipc::mem_ticket_t* ticket, uint64_t ticketId);


		// these are the internal declarations for the syscall functions.
		// we don't expose these to the user anyway, they're just there for
		// the syscall function table to work.
		void sc_null();

		void sc_exit(int status);
		int64_t sc_ipc_send(uint64_t target, ipc::message_body_t* msg);
		size_t sc_ipc_poll();
		void sc_ipc_discard();

		// returns the sender ID if there was a message, else 0.
		uint64_t sc_ipc_peek(ipc::message_body_t* msg);
		uint64_t sc_ipc_receive(ipc::message_body_t* msg);

		// returns the old handler.
		void* sc_ipc_set_signal_handler(uint64_t sigType, void* new_handler);


		void* sc_mmap_anon(void* req_addr, size_t length, int prot, int flags);
		void* sc_mmap_file(void* req_addr, size_t length, int prot, int flags, int fd, off_t offset);

		int sc_munmap(void* req_addr, size_t length);

		ssize_t sc_vfs_read(int fd, void* buf, size_t len);
		ssize_t sc_vfs_write(int fd, const void* buf, size_t len);

		void sc_user_signal_leave(uint64_t returnCode);

		void sc_log(int level, char* sys, size_t syslen, char* buf, size_t buflen);

		uint64_t sc_nanosecond_timestamp();
	}
}










