// rpc.h
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include "defs.h"
#include "synchro.h"
#include "export/rpc_message.h"


namespace nx::rpc
{
	struct Connection
	{
		uint64_t id;

		pid_t callerPid;
		pid_t calleePid;

		condvar<bool> inProgress;

		Synchronised<nx::list<rpc::message_t>, nx::mutex> calls;
		Synchronised<nx::list<rpc::message_t>, nx::mutex> results;
		condvar<bool> returnCV;

		bool sendCall(rpc::message_t msg, bool isVoid);
		bool sendResult(rpc::message_t msg);

		Connection() = default;
		Connection(Connection&&) = default;
		Connection& operator= (Connection&&) = default;

		Connection(const Connection&) = delete;
		Connection& operator= (const Connection&) = delete;

		~Connection();
	};

	void init();

	Connection* getConnection(uint64_t id);
	uint64_t createConnection(pid_t caller, pid_t callee);
	void closeConnection(uint64_t id);
}
