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
	syscall::kernel_log(0, "lmao", 4, "OMG", 3); // (%lu, %lu, %lu, %lu, %lu)\n", sender, type, a, b, c);

	uint32_t* fb = (uint32_t*) 0xFF'0000'0000;
	for(int y = 20; y < 40; y++)
	{
		for(int x = 1420; x < 1440; x++)
		{
			*(fb + y * 1440 + x) = 0xff'ffffff;
		}
	}
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

	int ctr2 = 0;
	constexpr size_t numColours = sizeof(colours) / sizeof(uint32_t);

	// these are in nanoseconds.
	uint64_t last_update = 0;
	constexpr uint64_t update_time = 250'000'000;


	while(true)
	{
		if(auto ts = syscall::nanosecond_timestamp(); last_update + update_time < ts)
		{
			last_update = ts;

			ctr2++;
			uint32_t* fb = (uint32_t*) 0xFF'0000'0000;
			for(int y = 0; y < 20; y++)
			{
				for(int x = 1420; x < 1440; x++)
				{
					*(fb + y * 1440 + x) = colours[ctr2 % numColours];
				}
			}
		}
	}
}








