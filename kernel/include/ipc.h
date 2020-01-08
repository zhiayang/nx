// ipc.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once
#include "defs.h"

#include "export/ipc_message.h"

namespace nx
{
	namespace ipc
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

		constexpr uint64_t SIGNAL_NORMAL    = 0;
		constexpr uint64_t SIGNAL_POSIX     = 1;
		constexpr uint64_t SIGNAL_TERMINATE = 2;

		constexpr uint64_t MAX_SIGNAL_TYPES = 32;

		// params: senderId, msg->sigType, msg->a, msg->b, msg->c.
		using signal_handler_fn_t = void (*)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

		void signalProcess(scheduler::Process* proc, uint64_t sigType, const signal_message_t& msg);
		void signalProcessCritical(scheduler::Process* proc, uint64_t sigType, const signal_message_t& msg);
	}
}










