// syscall.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once
#include "defs.h"

namespace nx
{
	namespace syscall
	{
		void init();

		constexpr uint64_t SYSCALL_EXIT         = 0;
		constexpr uint64_t SYSCALL_IPC_SEND     = 1;


		int64_t sc_exit(int status);        // SYSCALL_EXIT
		int64_t sc_ipc_send(void* msg);     // SYSCALL_IPC_SEND
	}
}
