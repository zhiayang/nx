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
	int send(uint64_t target, uint64_t a, uint64_t b, uint64_t c, uint64_t d);

	// peek a message from the queue; returns 0 if there is no message. else, returns the senderId
	// of the message sender.
	uint64_t peek(uint64_t* a, uint64_t* b, uint64_t* c, uint64_t* d);

	// same as peek, but will remove the message from the pending queue.
	uint64_t receive(uint64_t* a, uint64_t* b, uint64_t* c, uint64_t* d);

	// returns the number of messages in the queue.
	size_t poll();

	// discards one message from the front of the message queue. no action is taken if the queue is empty.
	void discard();

	// installs a handler for the given interrupt-signal type. returns the old handler, or -1 on error.
	void* install_intr_signal_handler(uint64_t sigType, signal_handler_fn_t handler);
}
}
