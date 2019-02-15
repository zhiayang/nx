// nxpc.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx/ipc.h"
#include "orionx/syscall.h"

namespace nx {
namespace ipc
{
	int send(message_t* msg)
	{
		// todo: do more advanced checks

		if(msg->magic != MAGIC_LE)                  return -1;
		if((msg->version & 0xFF) != CUR_VERSION)    return -1;

		// todo: figure out a proper way of defining the syscall numbers
		// preferably without #defines polluting the kernel namespace

		int64_t ret = 0;
		__nx_syscall_1(1, ret, msg);

		return ret;
	}

	bool poll(uint64_t classFilter, uint64_t senderFilter)
	{
		return false;
	}

	void receive(message_t* msg, uint64_t classFilter, uint64_t senderFilter)
	{
	}
}
}
