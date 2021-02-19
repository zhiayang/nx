// syscall.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once
#include "defs.h"
#include "export/ipc_message.h"
#include "export/rpc_message.h"
#include "export/syscall_funcs.h"

namespace nx
{
	namespace syscall
	{
		void init();

		bool copy_to_kernel(void* kernel, const void* user, size_t len);
		bool copy_to_user(void* user, const void* kernel, size_t len);

		uint64_t create_memory_ticket(size_t len, uint64_t flags);
		void collect_memory_ticket(ipc::mem_ticket_t* ticket, uint64_t ticketId);
		void find_existing_memory_ticket(ipc::mem_ticket_t* ticket, uint64_t ticketId);
		void release_memory_ticket(const ipc::mem_ticket_t* ticket);


		// these are the internal declarations for the syscall functions.
		// we don't expose these to the user anyway, they're just there for
		// the syscall function table to work.
		void do_nothing();

		pid_t spawn_process_from_memory(void* buf, size_t len);

		void exit(int status);
		uint64_t ipc_find_selector(ipc::selector_t sel);
		int64_t ipc_send(uint64_t target, ipc::message_body_t* msg);
		size_t ipc_poll();
		void ipc_discard();

		// returns the sender ID if there was a message, else 0.
		uint64_t ipc_peek(ipc::message_body_t* msg);
		uint64_t ipc_receive(ipc::message_body_t* msg);

		void rpc_call(uint64_t connId, const rpc::message_t* params, rpc::message_t* result);
		uint32_t rpc_call_procedure(uint64_t connId, const rpc::message_t* params);
		void rpc_return(uint64_t connId, const rpc::message_t* result);
		void rpc_wait_call(uint64_t connId, rpc::message_t* call_params);
		uint64_t rpc_wait_any_call(rpc::message_t* call_params);
		uint64_t rpc_open(uint64_t target);
		void rpc_close(uint64_t connId);

		// returns the old handler.
		void* ipc_set_signal_handler(uint64_t sigType, void* new_handler);

		void ipc_signal(ipc::selector_t sel, uint64_t a, uint64_t b, uint64_t c);
		void ipc_signal_block(ipc::selector_t sel, uint64_t a, uint64_t b, uint64_t c);

		void* mmap_anon(void* req_addr, size_t length, int prot, int flags);
		void* mmap_file(void* req_addr, size_t length, int prot, int flags, int fd, off_t offset);

		int munmap(void* req_addr, size_t length);

		ssize_t vfs_read(int fd, void* buf, size_t len);
		ssize_t vfs_write(int fd, const void* buf, size_t len);

		void user_signal_leave(uint64_t returnCode);

		void kernel_log(int level, char* sys, size_t syslen, char* buf, size_t buflen);

		uint64_t nanosecond_timestamp();
	}
}










