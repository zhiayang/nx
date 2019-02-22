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

			void* payload;
			size_t payloadSize;
		};

		void init();

		void addMessage(uint64_t senderId, uint64_t targetId, void* payload, size_t len);
		void disposeMessage(message_t& message);
	}
}










