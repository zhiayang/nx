// placebo.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <orionx/syscall.h>

#include <nxpc.h>

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

			int ret;
			__nx_syscall_1(6, ret, 'a' + nx::ipc::poll());

			nx::ipc::message_t msg;
			memset(&msg, 0, sizeof(nx::ipc::message_t));

			msg.magic = nx::ipc::MAGIC_LE;
			msg.targetId = 0;
			msg.version = nx::ipc::CUR_VERSION;

			nx::ipc::send(&msg);
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
			__nx_syscall_1(0, ret, 103);
		}
	}
}








