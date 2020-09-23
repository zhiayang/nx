// sc_rpc.cpp
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "rpc.h"

namespace nx::syscall
{
	void rpc_call(uint64_t connId, const rpc::message_t* params, rpc::message_t* result)
	{
		rpc::message_t input = { };

		auto rpc_failure = [&](uint32_t status) -> void {
			copy_to_user((uint32_t*) (((uint8_t*) result) + offsetof(rpc::message_t, status)),
				&status, sizeof(uint32_t));
		};

		auto conn = rpc::getConnection(connId);
		if(conn == nullptr)
			return rpc_failure(rpc::RPC_ERR_NO_CONNECTION);

		if(!copy_to_kernel(&input, params, sizeof(rpc::message_t)))
			return rpc_failure(rpc::RPC_ERR_INVALID_ARGS);

		if(!conn->sendCall(krt::move(input), /* isVoid: */ false))
			return rpc_failure(rpc::RPC_ERR_WRONG_CONNECTION);

		// wait for it to finish
		conn->inProgress.wait(false);

		conn->results.perform([&](auto& list) {
			assert(list.size() > 0);
			auto ret = list.popFront();

			copy_to_user(result, &ret, sizeof(rpc::message_t));
		});

		// done.
		return;
	}

	uint32_t rpc_call_void(uint64_t connId, const rpc::message_t* params)
	{
		rpc::message_t input = { };

		auto conn = rpc::getConnection(connId);
		if(conn == nullptr)
			return rpc::RPC_ERR_NO_CONNECTION;

		if(!copy_to_kernel(&input, params, sizeof(rpc::message_t)))
			return rpc::RPC_ERR_INVALID_ARGS;

		if(!conn->sendCall(krt::move(input), /* isVoid: */ true))
			return rpc::RPC_ERR_WRONG_CONNECTION;

		return rpc::RPC_OK;
	}

	void rpc_return(uint64_t connId, const rpc::message_t* result)
	{
		auto conn = rpc::getConnection(connId);
		if(conn == nullptr)
			return;

		rpc::message_t msg = { };
		if(!copy_to_kernel(&msg, result, sizeof(rpc::message_t)))
			return;

		conn->sendResult(krt::move(msg));
	}

	void rpc_wait_call(uint64_t connId, rpc::message_t* call_params)
	{
		auto conn = rpc::getConnection(connId);
		if(conn == nullptr)
			return;

		// wait till there's something in progress
		conn->inProgress.wait(true);
		conn->calls.perform([call_params](auto& list) {
			assert(list.size() > 0);

			auto ret = list.popFront();
			copy_to_user(call_params, &ret, sizeof(rpc::message_t));
		});
	}

	uint64_t rpc_wait_any_call(rpc::message_t* call_params)
	{
		auto proc = scheduler::getCurrentProcess();
		auto connId = proc->rpcCalleePending.pop();

		auto conn = rpc::getConnection(connId);
		if(conn == nullptr)
			return 0;

		rpc_wait_call(connId, call_params);
		return connId;
	}

	uint64_t rpc_open(uint64_t target)
	{
		auto callee = scheduler::getProcessWithId(target);
		if(!callee)
		{
			warn("rpc", "tried to open rpc to nonexistent pid %lu", target);
			return 0;
		}

		auto caller = scheduler::getCurrentProcess();
		assert(caller);

		auto conn = rpc::createConnection(caller->processId, callee->processId);
		assert(conn != 0);

		return conn;
	}

	void rpc_close(uint64_t connId)
	{
		rpc::closeConnection(connId);
	}
}
