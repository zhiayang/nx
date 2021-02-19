// syscall_funcs.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once
#include <stdint.h>

#ifdef __cplusplus

namespace nx
{
	constexpr uint64_t SYSCALL_EXIT                         = 0;

	constexpr uint64_t SYSCALL_IPC_SEND                     = 1;
	constexpr uint64_t SYSCALL_IPC_PEEK                     = 2;
	constexpr uint64_t SYSCALL_IPC_POLL                     = 3;
	constexpr uint64_t SYSCALL_IPC_DISCARD                  = 4;
	constexpr uint64_t SYSCALL_IPC_RECEIVE                  = 5;
	constexpr uint64_t SYSCALL_IPC_RECEIVE_BLOCK            = 6;
	constexpr uint64_t SYSCALL_IPC_SET_SIG_HANDLER          = 7;
	constexpr uint64_t SYSCALL_IPC_SIGNAL                   = 8;
	constexpr uint64_t SYSCALL_IPC_SIGNAL_BLOCK             = 9;
	constexpr uint64_t SYSCALL_IPC_FIND_SELECTOR            = 10;

	constexpr uint64_t SYSCALL_RPC_CALL                     = 11;
	constexpr uint64_t SYSCALL_RPC_CALL_PROCEDURE           = 12;
	constexpr uint64_t SYSCALL_RPC_RETURN                   = 13;
	constexpr uint64_t SYSCALL_RPC_WAIT_CALL                = 14;
	constexpr uint64_t SYSCALL_RPC_WAIT_ANY_CALL            = 15;
	constexpr uint64_t SYSCALL_RPC_OPEN                     = 16;
	constexpr uint64_t SYSCALL_RPC_CLOSE                    = 17;
	constexpr uint64_t SYSCALL_RPC_FORWARD                  = 18;

	constexpr uint64_t SYSCALL_MMAP_ANON                    = 20;
	constexpr uint64_t SYSCALL_MMAP_FILE                    = 21;

	constexpr uint64_t SYSCALL_MEMTICKET_CREATE             = 30;
	constexpr uint64_t SYSCALL_MEMTICKET_COLLECT            = 31;
	constexpr uint64_t SYSCALL_MEMTICKET_RELEASE            = 32;
	constexpr uint64_t SYSCALL_MEMTICKET_FIND               = 33;

	constexpr uint64_t SYSCALL_PROCESS_SPAWN_FROM_MEMORY    = 40;

	constexpr uint64_t SYSCALL_VFS_READ                     = 80;
	constexpr uint64_t SYSCALL_VFS_WRITE                    = 81;

	constexpr uint64_t SYSCALL_GET_NANOSECOND_TS            = 97;
	constexpr uint64_t SYSCALL_USER_SIGNAL_LEAVE            = 98;
	constexpr uint64_t SYSCALL_LOG                          = 99;

	constexpr uint64_t _SYSCALL_MAX                         = 100;
}

#endif
