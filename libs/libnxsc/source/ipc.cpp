// ipc.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <syscall.h>
#include <nx/thread_local.h>

#include "ipc.h"

namespace nx {
namespace ipc
{
	int send(message_t* msg, size_t len)
	{
		// todo: do more advanced checks

		if(msg->magic != MAGIC_LE)                  return -1;
		if((msg->version & 0xFF) != CUR_VERSION)    return -1;

		int64_t ret = 0;
		__nx_syscall_1(SYSCALL_IPC_SEND, ret, msg);

		return ret;
	}

	void discard()
	{
		__nx_syscall_0v(SYSCALL_IPC_DISCARD);
	}

	size_t peek(message_t* msg, size_t len)
	{
		size_t ret = 0;
		__nx_syscall_2(SYSCALL_IPC_PEEK, ret, msg, len);

		return ret;
	}

	size_t receive(message_t* msg, size_t len)
	{
		size_t ret = 0;
		__nx_syscall_2(SYSCALL_IPC_RECEIVE, ret, msg, len);

		return ret;
	}

	size_t poll()
	{
		size_t ret = 0;
		__nx_syscall_0(SYSCALL_IPC_POLL, ret);

		return ret;
	}


	message_t* init_msg(message_t* buf)
	{
		if(!buf) return 0;

		memset(buf, 0, sizeof(message_t));

		buf->magic          = MAGIC_LE;
		buf->version        = 1;
		buf->messageClass   = CLASS_EMPTY;

		return buf;
	}
}
}



















