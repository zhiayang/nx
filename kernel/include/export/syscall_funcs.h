// syscall_funcs.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once
#include <stdint.h>

#ifdef __cplusplus

namespace nx
{
	constexpr uint64_t SYSCALL_EXIT         = 0;
	constexpr uint64_t SYSCALL_IPC_SEND     = 1;
	constexpr uint64_t SYSCALL_IPC_PEEK     = 2;
	constexpr uint64_t SYSCALL_IPC_POLL     = 3;
	constexpr uint64_t SYSCALL_IPC_DISCARD  = 4;
	constexpr uint64_t SYSCALL_IPC_RECEIVE  = 5;
}

#endif
