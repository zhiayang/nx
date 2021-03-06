// rpc.cpp
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "rpc.h"

#include "krt/types/hashset.h"

namespace nx::rpc
{
	static uint64_t nextConnId = 0;
	static Synchronised<nx::bucket_hashmap<uint64_t, Connection>, nx::mutex> connectionList;

	void init()
	{
		connectionList = decltype(connectionList)();
	}

	uint64_t createConnection(pid_t caller, pid_t callee)
	{
		auto proc = scheduler::getProcessWithId(callee);
		assert(proc);

		auto conn = Connection();
		conn.callerPid  = caller;
		conn.calleePid  = callee;
		conn.id         = ++nextConnId;

		auto ret = conn.id;
		connectionList.perform([&conn](auto& list) {
			list[conn.id] = krt::move(conn);
		});

		proc->rpcCalleeConnections.lock()->insert(ret);
		log("rpc", "created connection ({}): {} -> {}", ret, caller, callee);

		return ret;
	}

	Connection* getConnection(uint64_t id)
	{
		return connectionList.map([&id](auto& list) -> Connection* {
			if(auto it = list.find(id); it != list.end())
				return &it->value;

			warn("rpc", "tried to find invalid connection {}", id);
			return nullptr;
		});
	}

	void closeConnection(uint64_t id)
	{
		connectionList.perform([&id](auto& list) {
			if(auto it = list.find(id); it != list.end())
			{
				dbg("rpc", "closing connection {}", id);

				auto proc = scheduler::getProcessWithId(it->value.calleePid);
				if(proc)
					proc->rpcCalleeConnections.lock()->remove(it->value.id);

				list.erase(it);
			}
			else
			{
				log("rpc", "invalid connection {}", id);
			}
		});
	}

	Connection::~Connection()
	{

	}

	bool Connection::sendCall(message_t msg, bool isVoid)
	{
		if(auto p = scheduler::getCurrentProcess()->processId; p != this->callerPid)
		{
			error("rpc", "pid {} is not the designated caller of rpc_conn {}", p, this->id);
			return false;
		}

		msg.counterpart = scheduler::getCurrentProcess()->processId;
		msg.sequence = this->seq++;
		msg.status = RPC_OK;

		dbg("rpc", "call {} ({} -> {})", this->id, msg.counterpart, this->calleePid);

		// wait for existing things to finish
		this->inProgress.wait(false);
		this->calls.lock()->append(krt::move(msg));

		// if we expect a result, then we need to set the inProgress flag. if not, then it is
		// a simple fire-and-forget thing.
		if(!isVoid)
			this->inProgress.set(true);

		// wake up the callee if necessary
		auto callee = scheduler::getProcessWithId(this->calleePid);
		assert(callee);

		callee->rpcCalleePending.push_if_waiting(this->id);
		return true;
	}

	bool Connection::sendResult(message_t msg)
	{
		//* we check the callerPid for calls, but not the calleePid for returns. this is to
		//* allow a server (eg. A) to forward client calls to another server (eg. B), so that
		//* B can return data directly to the client; instead of C -> A -> B -> A -> C, we
		//* can 'just' do C -> A -> B -> C.

		// TODO:
		// for now, there is no mechanism to "transfer" the connection to another server, so
		// that we can potentially do C -> A -> B -> C first, but then only C -> B -> C for
		// subsequent calls -- as long as both A and B agree on some plumbing.

		msg.counterpart = scheduler::getCurrentProcess()->processId;
		msg.sequence = this->seq++;

		// there must be something in progress...
		assert(this->inProgress.get());

		this->results.lock()->append(krt::move(msg));
		dbg("rpc", "return {} ({} <- {})", this->id, this->callerPid, msg.counterpart);

		// notify that we're done.
		this->inProgress.set(false);
		return true;
	}
}


