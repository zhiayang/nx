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

			void* payload;
			size_t payloadSize;
		};

		void init();

		void addMessage(const message_t& msg, bool makeCopy = true);
		void disposeMessage(message_t& message);

		// no flag.
		constexpr uint64_t MSG_FLAG_NONE        = 0x00;

		// indicates that the incoming message was a signal message
		constexpr uint64_t MSG_FLAG_SIGNAL      = 0x01;

		// indicates that we should free the payload on disposal. users don't get to set this.
		// anyway, the message_t struct isn't even exposed to userspace, so it's fine.
		constexpr uint64_t MSG_FLAG_OWNED_MEM   = 0x02;



		// this is completely (kinda) separate from posix signal stuff.
		// this "signalling" refers to IPC messages that will interrupt the target thread/process,
		// instead of enqueuing it and relying on the user process to poll for messages.

		constexpr uint64_t SIGNAL_NONE      = 0;
		constexpr uint64_t SIGNAL_POSIX     = 1;
		constexpr uint64_t SIGNAL_TERMINATE = 2;

		constexpr uint64_t MAX_SIGNAL_TYPES = 32;

		// params: &senderId, buffer, size. return: some kind of status?
		using signal_handler_fn_t = int (*)(uint64_t*, void*, size_t);

		// if the incoming IPC message is a signal (ie. msg->flags & MSG_FLAG_SIGNAL), then the
		// first 8 bytes of the message payload is a uint64_t indicating the signal type. the
		// rest of the message payload is user-defined data.

		void signalProcess(scheduler::Process* proc, uint64_t sigType, const message_t& msg);
		void signalProcessCritical(scheduler::Process* proc, uint64_t sigType, const message_t& msg);
	}
}










