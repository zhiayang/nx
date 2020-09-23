// nxsc_rpc.cpp
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "rpc.h"
#include <nx/syscall.h>

namespace nx
{
	rpc::message_t rpc_call(uint64_t connId, const rpc::message_t& params)
	{
		rpc::message_t result = { };
		__nx_syscall_3v(SYSCALL_RPC_CALL, connId, &params, &result);

		return result;
	}

	uint32_t rpc_call_void(uint64_t connId, const rpc::message_t& params)
	{
		uint64_t ret = 0;
		__nx_syscall_2(SYSCALL_RPC_CALL_VOID, ret, connId, &params);

		return (uint32_t) ret;
	}

	void rpc_return(uint64_t connId, const rpc::message_t& result)
	{
		__nx_syscall_2v(SYSCALL_RPC_RETURN, connId, &result);
	}

	rpc::message_t rpc_wait_call(uint64_t connId)
	{
		rpc::message_t msg = { };
		__nx_syscall_2v(SYSCALL_RPC_WAIT_CALL, connId, &msg);

		return msg;
	}

	rpc::message_t rpc_wait_any_call(uint64_t* connId)
	{
		uint64_t conn = 0;
		rpc::message_t msg = { };

		__nx_syscall_1(SYSCALL_RPC_WAIT_ANY_CALL, conn, &msg);
		if(connId)
			*connId = conn;

		return msg;
	}

	uint64_t rpc_open(uint64_t target)
	{
		uint64_t ret = 0;
		__nx_syscall_1(SYSCALL_RPC_OPEN, ret, target);

		return ret;
	}

	void rpc_close(uint64_t connId)
	{
		__nx_syscall_1v(SYSCALL_RPC_CLOSE, connId);
	}
}
