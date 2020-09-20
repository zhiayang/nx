// ipc.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once
#include "defs.h"

#include "export/ipc_message.h"

namespace nx::ipc
{
	struct message_t
	{
		uint64_t senderId;
		uint64_t targetId;

		uint64_t flags;
		message_body_t body;
	};

	struct signal_message_t
	{
		uint64_t senderId;
		uint64_t targetId;

		uint64_t flags;
		signal_message_body_t body;

		condvar<bool>* blocking_cv;
	};

	void init();

	void addMessage(const message_t& msg);
	void disposeMessage(message_t& message);

	// no flag.
	constexpr uint64_t MSG_FLAG_NONE        = 0x00;

	// indicates that the incoming message was a signal message
	constexpr uint64_t MSG_FLAG_SIGNAL      = 0x01;


	// this is completely (kinda) separate from posix signal stuff.
	// this "signalling" refers to IPC messages that will interrupt the target thread/process,
	// instead of enqueuing it and relying on the user process to poll for messages.

	constexpr uint64_t MAX_SIGNAL_TYPES = 32;

	bool signal(scheduler::Thread* thr, uint64_t sigType, const signal_message_body_t& msg);
	bool signal(const selector_t& sel, uint64_t sigType, const signal_message_body_t& msg);
	bool signalBlocking(const selector_t& sel, uint64_t sigType, const signal_message_body_t& msg, condvar<bool>* cv);

	uint64_t createMemticket(size_t len, uint64_t flags);
	void releaseMemticket(const mem_ticket_t& ticket);
	mem_ticket_t collectMemticket(uint64_t ticketId);
	mem_ticket_t collectOrReuseMemticket(uint64_t ticketId);

	// TODO: for now, again we can only send messages/signals to threads, not processes.
	scheduler::Thread* resolveSelector(const selector_t& sel);
}










