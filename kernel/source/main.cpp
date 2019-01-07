// main.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"



namespace nx
{
	void kernel_main(BootInfo* bootinfo)
	{
		// setup all of our memory facilities.
		pmm::init(bootinfo);
		vmm::init(bootinfo);
		heap::init();

		// initialise the vfs so we can read the initrd
		vfs::init();

		// mount the tarfs at /initrd
		initrd::init(bootinfo);

		{
			// test stuff!
			auto f = vfs::open("/initrd/misc/LICENSE.md", vfs::Mode::Read);

			auto buf = new uint8_t[5000];
			auto r = vfs::read(f, buf, 5000);

			println("read %zu bytes\n", r);
			println("dump: %s\n", buf);
		}
	}
}


















extern "C" void kernel_main(nx::BootInfo* bootinfo)
{
	nx::println("[nx] kernel has control");
	nx::println("bootloader ident: '%c%c%c'\n", bootinfo->ident[0], bootinfo->ident[1], bootinfo->ident[2]);

	nx::kernel_main(bootinfo);


	nx::println("\nnothing to do...");

	while(true)
		;
}

















