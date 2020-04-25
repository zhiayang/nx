// dispatcher.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace ipc
{
	void init()
	{
	}

	void addMessage(const message_t& umsg)
	{
		auto msg = message_t();

		msg.flags       = umsg.flags;
		msg.senderId    = umsg.senderId;
		msg.targetId    = umsg.targetId;
		memcpy(msg.body.bytes, umsg.body.bytes, umsg.body.BufferSize);

		if(auto proc = scheduler::getProcessWithId(msg.targetId); proc)
			proc->pendingMessages.lock()->append(msg);
	}

	void disposeMessage(message_t& message)
	{
		// there's nothing to do here lmao
	}
}
}


































