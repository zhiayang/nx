// placebo.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include <nx/ipc.h>
#include <nx/rpc.h>
#include <nx/syscall.h>

#include <svr/tty.h>
#include <svr/vfs.h>

static constexpr int WIDTH = 1440;
uint64_t sig_handler(uint64_t sender, uint64_t type, uint64_t a, uint64_t b, uint64_t c)
{
	syscall::kernel_log(0, "lmao", 4, "OMG", 3); // (%lu, %lu, %lu, %lu, %lu)\n", sender, type, a, b, c);

	uint32_t* fb = (uint32_t*) 0xFF'0000'0000;
	for(int y = 80; y < 160; y++)
	{
		for(int x = WIDTH - 80; x < WIDTH; x++)
		{
			*(fb + y * WIDTH + x) = 0xff'ffffff;
		}
	}

	return 0;
}


uint64_t test_handler(uint64_t sender, uint64_t type, uint64_t a, uint64_t b, uint64_t c)
{
	printf("stalling for time...\n");
	auto end = syscall::nanosecond_timestamp() + 2'500'000'000;

	while(syscall::nanosecond_timestamp() < end)
		;

	printf("finished stalling\n");
	return 0;
}

volatile int* foozle = (int*) 0x5555'0000'0000'0000;
int fn3()
{
	return 37 * (*foozle = 300);
}

int fn2()
{
	return 7 * fn3();
}

int fn1()
{
	return 9 - fn2();
}

int main()
{
	nx::ipc::install_intr_signal_handler(nx::ipc::SIGNAL_TERMINATE, &sig_handler);
	nx::ipc::install_intr_signal_handler(nx::ipc::SIGNAL_NORMAL, &test_handler);
	printf("time for uwu\n");

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

	printf("time for owo\n");

	{
		syscall::kernel_log(2, "placebo", 7, "trying to vfs...", 16);

		using namespace nx;

		auto sel = "/proc/name/vfs-svr";
		auto id = ipc::find_selector(ipc::selector_t(sel));
		if(id == (uint64_t) -1)
		{
			printf("could not find vfs-svr\n");
			abort();
		}

		auto tmp = ipc::collect_memory_ticket(ipc::create_memory_ticket(1024, 0));
		assert(tmp.ptr != nullptr);

	#if 0
		auto path = "/initrd/boot/bfx_kernel_params";
		auto plen = strlen(path);

		memcpy(tmp.ptr, path, plen);

		{
			auto client = rpc::Client(id);
			assert(client.valid());

			auto res = client.call<vfs::fns::Register>(tmp.ticketId);
			if(!res) { printf("register failed\n"); }
			else     { printf("register ok\n"); }

			auto handle = client.call<vfs::fns::Open>(/* flags: */ 0,
				vfs::Buffer {
					.memTicketId = tmp.ticketId,
					.offset = 0,
					.length = plen
				}
			);

			printf("** received handle: %lu\n", handle->id);
		}


		ipc::release_memory_ticket(tmp);
	#endif
	}




	while(true)
	{
		auto ts = syscall::nanosecond_timestamp();

		if(last_update + update_time < ts)
		{
			last_update = ts;

			ctr2++;
			uint32_t* fb = (uint32_t*) 0xFF'0000'0000;
			for(int y = 0; y < 80; y++)
			{
				for(int x = WIDTH - 80; x < WIDTH; x++)
				{
					*(fb + y * WIDTH + x) = colours[ctr2 % numColours];
				}
			}
		}

		if(nx::ipc::poll() > 0)
		{
			nx::ipc::message_body_t body;
			nx::ipc::receive(&body);

			auto x = nx::ipc::extract<uint64_t>(body);
			(void) x;

			auto ticket = nx::ipc::collect_memory_ticket(1);
			memcpy((void*) 0xFF'0000'0000, ticket.ptr, ticket.len);

			// printf("%5d signalled! (%llu) %p, %zu\n", foozle++, x, ticket.ptr, ticket.len);
			// printf(".");
			syscall::kernel_log(0, "", 0, "!", 1);

			nx::ipc::release_memory_ticket(ticket);

			// crash
			// printf("foozle = %d\n", fn1());
		}
	}
}








