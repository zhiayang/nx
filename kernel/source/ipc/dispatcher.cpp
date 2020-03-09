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

		void addMessage(const message_t& umsg)
		{
			auto msg = message_t();

			msg.flags       = umsg.flags;
			msg.senderId    = umsg.senderId;
			msg.targetId    = umsg.targetId;
			memcpy(msg.body.bytes, umsg.body.bytes, umsg.body.BufferSize);

			LockedSection(&queueLock, [&msg]() {
				messageQueue.append(msg);
			});
		}

		void disposeMessage(message_t& message)
		{
			// there's nothing to do here lmao
		}
	}
}


































