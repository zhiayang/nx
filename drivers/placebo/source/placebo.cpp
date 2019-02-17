// placebo.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <ipc.h>
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
		if(ctr++ % 50000000 == 0)
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

					char buf[sizeof(message_t) + strlen(str) + 1];

					auto msg = init_msg((message_t*) buf);

					msg->payloadSize = strlen(str);
					memcpy(msg->payload, str, strlen(str) + 1);

					send(msg, sizeof(message_t) + msg->payloadSize + 1);
				}

				while(!poll())
					;

				auto sz = receive(NULL, 0);
				char buf[sz] = { };

				receive((message_t*) buf, sz);

				__nx_syscall_1v(8, ((message_t*) buf)->payload);
			}
		}

		if(ctr2 == 16)
		{
			uint32_t* fb = (uint32_t*) 0xFFFF'FFFF'0000'0000;
			for(int y = 0; y < 300; y++)
			{
				for(int x = 0; x < 800; x++)
				{
					*(fb + y * 800 + x) = 0;
				}
			}

			int ret;
			__nx_syscall_1(nx::SYSCALL_EXIT, ret, 103);
		}
	}
}








