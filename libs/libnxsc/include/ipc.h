// ipc.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include <stdint.h>

#include <nx/ipc_message.h>

namespace nx {
namespace ipc
{
	// sends an ipc message
	int send(uint64_t target, void* msg, size_t len);

	// peek a message from the queue; returns 0 if there is no message. if 'msg' is NULL,
	// returns the length of the buffer required to read the entire message. messages cannot be read partially.
	size_t peek(void* msg, size_t len);

	// same as peek, but will remove the message from the pending queue. if 'msg' is NULL, the message
	// will not be discarded from the queue!
	size_t receive(void* msg, size_t len);

	// returns the number of messages in the queue.
	size_t poll();

	// discards one message from the front of the message queue. no action is taken if the queue is empty.
	void discard();
}
}
