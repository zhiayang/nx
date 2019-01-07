// main.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx
{
	void kernel_main(BootInfo* bootinfo)
	{
		// init the fallback console
		console::fallback::init(bootinfo->fbHorz, bootinfo->fbVert, bootinfo->fbScanWidth);
		console::fallback::foreground(0xFFE0E0E0);
		console::fallback::background(0);

		println("[nx] kernel has control");
		println("bootloader ident: '%c%c%c'\n", bootinfo->ident[0], bootinfo->ident[1], bootinfo->ident[2]);

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
			assert(f);

			auto buf = new uint8_t[8500];
			auto r = vfs::read(f, buf, 8500);

			println("read %zu bytes\n", r);
			println("dump: %s\n", buf);
		}
	}
}


















extern "C" void kernel_main(nx::BootInfo* bootinfo)
{
	nx::kernel_main(bootinfo);


	nx::println("\nnothing to do...");

	while(true)
		;
}

















