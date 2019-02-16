// dispatcher.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx
{
	namespace ipc
	{
		static nx::mutex queueLock;
		static nx::list<message_t*> messageQueue;
		static void dispatcher()
		{
			while(true)
			{
				while(messageQueue.size() > 0)
				{
					message_t* msg = 0;
					{
						autolock lk(&queueLock);
						msg = messageQueue.popFront();
					}

					assert(msg);
					log("ipc", "message from %lu", msg->senderId);


					// for now reply the mesage.
					if(auto proc = scheduler::getProcessWithId(msg->senderId); proc)
					{
						auto reply = new ((void*) heap::allocate(sizeof(message_t), alignof(message_t))) message_t;

						reply->magic = MAGIC_LE;
						reply->version = 1;
						reply->messageClass = 0;

						reply->senderId = 0;
						reply->targetId = msg->senderId;

						reply->payloadSize = 0;

						// TODO: @LockSafety lock this!
						proc->pendingMessages.append(reply);
					}
				}

				scheduler::yield();
			}
		}

		void init()
		{
			queueLock = nx::mutex();
			messageQueue = nx::list<message_t*>();

			scheduler::addThread(scheduler::createThread(scheduler::getKernelProcess(), dispatcher));
		}

		void addMessage(message_t* msg)
		{
			// note: we assert here, because the syscall handler should have verified this already.
			assert(msg->magic == MAGIC_LE);
			assert((msg->version & 0xFF) == CUR_VERSION);

			// ok then...
			auto copy = (message_t*) heap::allocate(sizeof(message_t) + msg->payloadSize, alignof(message_t));
			memcpy(copy, msg, sizeof(message_t) + msg->payloadSize);

			{
				autolock lk(&queueLock);
				messageQueue.append(copy);
			}
		}
	}
}


































