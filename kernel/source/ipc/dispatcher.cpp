// dispatcher.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx
{
	namespace ipc
	{
		static nx::mutex queueLock;
		static nx::list<message_t> messageQueue;

		static void dispatcher()
		{
			while(true)
			{
				while(messageQueue.size() > 0)
				{
					auto msg = message_t();
					{
						autolock lk(&queueLock);
						msg = messageQueue.popFront();
					}

					if(auto proc = scheduler::getProcessWithId(msg.targetId); proc)
					{
						autolock lk(&proc->msgQueueLock);
						proc->pendingMessages.append(msg);
					}
				}

				scheduler::yield();
			}
		}

		void init()
		{
			queueLock = nx::mutex();
			messageQueue = nx::list<message_t>();

			scheduler::addThread(scheduler::createThread(scheduler::getKernelProcess(), dispatcher));
		}

		void addMessage(uint64_t senderId, uint64_t targetId, void* payload, size_t len)
		{
			// ok then...
			auto copy = (void*) heap::allocate(len, 1);
			memcpy(copy, payload, len);

			auto msg = message_t();
			msg.payload = copy;
			msg.payloadSize = len;

			msg.senderId = senderId;
			msg.targetId = targetId;

			{
				autolock lk(&queueLock);
				messageQueue.append(msg);
			}
		}

		void disposeMessage(message_t& message)
		{
			heap::deallocate((addr_t) message.payload);
		}
	}
}


































