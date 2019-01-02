// main.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"



namespace nx
{
	void kernel_main(BootInfo* bootinfo)
	{
		// clear the screen
		// krt::util::memfill4b((uint32_t*) bootinfo->frameBuffer, 0, 800 * 600);

		pmm::init(bootinfo);
		vmm::init(bootinfo);

		heap::init();

		{
			struct foo
			{
				int a;
				int b;
				int c;
				int d;
			};

			auto f = (foo*) heap::allocate(sizeof(foo), alignof(foo));
			println("f = %p (%zu, %zu)", f, sizeof(foo), alignof(foo));

			heap::deallocate((addr_t) f);
		}

		{
			auto x = heap::allocate(3 * PAGE_SIZE, 1);
			println("x = %p", x);

			heap::deallocate(x);
		}


		println("\nnothing to do...");
	}
}



















extern "C" void kernel_main(nx::BootInfo* bootinfo)
{
	nx::println("[nx] kernel has control");
	nx::println("bootloader ident: '%c%c%c'\n", bootinfo->ident[0], bootinfo->ident[1], bootinfo->ident[2]);

	// tell the world (kinda)
	nx::kernel_main(bootinfo);

	while(true)
		;
}