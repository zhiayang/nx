// placebo.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <ipc.h>
#include <svr/tty.h>
#include <syscall.h>

extern "C" int main()
{
	uint64_t ctr = 0;

	constexpr uint32_t colours[4] = {
		0xffff0000,
		0xff00ff00,
		0xff0000ff,
		0xff00ffff,
	};

	int ctr2 = 0;
	while(true)
	{
		if(ctr++ % 35000000 == 0)
		{
			ctr2++;
			uint32_t* fb = (uint32_t*) 0xFFFF'FFFF'0000'0000;
			for(int y = 0; y < 300; y++)
			{
				for(int x = 0; x < 800; x++)
				{
					*(fb + y * 800 + x) = colours[ctr2 % 4];
				}
			}

			{
				using namespace nx::ipc;
				{
					auto str = "hello there!";

					char buf[sizeof(message_t) + sizeof(ttysvr::payload_t) + strlen(str) + 1];

					auto msg = init_msg((message_t*) buf);
					msg->targetId = 1;

					msg->payloadSize = sizeof(ttysvr::payload_t) + strlen(str) + 1;
					auto ttypl = (ttysvr::payload_t*) msg->payload;

					ttypl->magic = ttysvr::MAGIC;
					ttypl->tty = 1;

					ttypl->dataSize = strlen(str) + 1;
					memcpy(ttypl->data, str, strlen(str));

					send(msg, sizeof(message_t) + msg->payloadSize);
				}
			}
		}

		if(ctr2 == 4)
		{
			ctr2 = 0;
			// printf("cycle\n");
		}

		// if(ctr2 == 12)
		// {
		// 	uint32_t* fb = (uint32_t*) 0xFFFF'FFFF'0000'0000;
		// 	for(int y = 0; y < 300; y++)
		// 	{
		// 		for(int x = 0; x < 800; x++)
		// 		{
		// 			*(fb + y * 800 + x) = 0;
		// 		}
		// 	}

		// 	// int ret;
		// 	// __nx_syscall_1(nx::SYSCALL_EXIT, ret, 103);
		// }

	}
}








