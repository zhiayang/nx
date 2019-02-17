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

		void sc_exit(int status);
		int64_t sc_ipc_send(void* msg, size_t len);
		size_t sc_ipc_peek(void* msg, size_t len);
		size_t sc_ipc_poll();
		void sc_ipc_discard();
		size_t sc_ipc_receive(void* msg, size_t len);
	}
}
