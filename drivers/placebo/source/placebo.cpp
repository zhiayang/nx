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

void sig_handler(uint64_t sender, uint64_t type, uint64_t a, uint64_t b, uint64_t c)
{
	printf("received signal!!!! (%lu, %lu, %lu, %lu, %lu)\n", sender, type, a, b, c);
}


extern "C" int main()
{
	nx::ipc::install_intr_signal_handler(nx::ipc::SIGNAL_TERMINATE, &sig_handler);

	uint64_t ctr = 0;

	constexpr uint32_t colours[] = {
		0xff'fdb813,
		0xff'f37020,
		0xff'c9234b,
		0xff'635fab,
		0xff'0089cf,
		0xff'0ab04a,
	};

	constexpr size_t numColours = sizeof(colours) / sizeof(uint32_t);

	int ctr2 = 0;
	while(true)
	{
		if(ctr++ % 20000000 == 0)
		{
			ctr2++;
			uint32_t* fb = (uint32_t*) 0xFF'0000'0000;
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
					auto str = ".";

					size_t len = sizeof(ttysvr::payload_t) + strlen(str) + 1;
					char buf[len];
					memset(buf, 0, len);


					auto ttypl = (ttysvr::payload_t*) buf;

					ttypl->magic = ttysvr::MAGIC;
					ttypl->tty = 0;

					ttypl->dataSize = strlen(str) + 1;
					memcpy(ttypl->data, str, strlen(str));

					// send(1, buf, len);
				}
			}
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








