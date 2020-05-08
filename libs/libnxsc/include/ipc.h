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
	int send(uint64_t target, const message_body_t* msg);

	// peek a message from the queue; returns 0 if there is no message. else, returns the senderId
	// of the message sender.
	uint64_t peek(message_body_t* msg);

	// same as peek, but will remove the message from the pending queue.
	uint64_t receive(message_body_t* msg);

	// returns the number of messages in the queue.
	size_t poll();

	// discards one message from the front of the message queue. no action is taken if the queue is empty.
	void discard();

	// installs a handler for the given interrupt-signal type. returns the old handler, or -1 on error.
	void* install_intr_signal_handler(uint64_t sigType, signal_handler_fn_t handler);


	// helper functions.
	template <typename T>
	T extract(const message_body_t& body)
	{
		static_assert(sizeof(T) <= message_body_t::BufferSize, "type is too large to fit");
		return *reinterpret_cast<const T*>(body.bytes);
	}

	template <typename T>
	int send(uint64_t target, const T& x)
	{
		static_assert(sizeof(T) <= message_body_t::BufferSize, "type is too large to fit");
		return send(target, reinterpret_cast<const message_body_t*>(&x));
	}

	void signal(const selector_t& sel, uint64_t a, uint64_t b, uint64_t c);
	void signal_block(const selector_t& sel, uint64_t a, uint64_t b, uint64_t c);

	uint64_t create_memory_ticket(size_t len, uint64_t flags);
	mem_ticket_t collect_memory_ticket(uint64_t ticketId);
	void release_memory_ticket(const mem_ticket_t& ticket);
}
}










