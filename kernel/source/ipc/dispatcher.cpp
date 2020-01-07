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

		void addMessage(const message_t& umsg, bool makeCopy)
		{
			auto len = umsg.payloadSize;

			// ok then...
			void* buffer = 0;

			auto msg = message_t();

			msg.flags = umsg.flags;
			msg.payloadSize = len;

			if(makeCopy)
			{
				buffer = (void*) heap::allocate(len, 1);
				memcpy(buffer, umsg.payload, len);

				msg.flags |= MSG_FLAG_OWNED_MEM;
			}
			else
			{
				buffer = umsg.payload;
			}

			msg.payload     = buffer;
			msg.senderId    = umsg.senderId;
			msg.targetId    = umsg.targetId;

			{
				autolock lk(&queueLock);
				messageQueue.append(msg);
			}
		}

		void disposeMessage(message_t& message)
		{
			// only deallocate if we own the memory.
			if(message.flags & MSG_FLAG_OWNED_MEM)
				heap::deallocate((addr_t) message.payload);
		}
	}
}


































