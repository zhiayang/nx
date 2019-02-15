// dispatcher.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace ipc
{
	static nx::mutex messageQueueLock;
	static nx::list<message_t*> messageQueue;
	static void dispatcher()
	{
		while(true)
		{
			while(messageQueue.size() > 0)
			{
				message_t* msg = 0;
				messageQueueLock.lock();
				{
					msg = messageQueue.popFront();
				}
				messageQueueLock.unlock();

				assert(msg);
				log("ipc", "message from %lu", msg->senderId);
			}

			scheduler::yield();
		}
	}

	void init()
	{
		messageQueue = nx::list<message_t*>();
		messageQueueLock = nx::mutex();

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

		messageQueueLock.lock();
		{
			messageQueue.append(copy);
		}
		messageQueueLock.unlock();
	}
}

	int64_t syscall::sc_ipc_send(void* _msg)
	{
		using namespace nx::ipc;


		if(!_msg) return -1;

		auto msg = (message_t*) _msg;
		assert(msg);

		if(msg->magic != MAGIC_LE)
		{
			error("ipc", "invalid magic %x in message; discarding", msg->magic);
			return -1;
		}
		if((msg->version & 0xFF) != CUR_VERSION)
		{
			error("ipc", "invalid version %d in message; discarding", msg->version & 0xFF);
			return -1;
		}
		if((msg->version & ~0xFF) != 0)
		{
			error("ipc", "invalid feature flags %x in message; discarding", msg->version & ~0xFF);
			return -1;
		}
		if(msg->payloadSize > MAX_PAYLOAD_SIZE)
		{
			error("ipc", "payload size of %zu exceeds max of %zu; discarding", msg->payloadSize, MAX_PAYLOAD_SIZE);
			return -1;
		}

		msg->senderId = scheduler::getCurrentThread()->threadId;

		// ok, add it
		addMessage(msg);

		return 0;
	}
}


































