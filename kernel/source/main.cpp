// main.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx
{
	void kernel_main(BootInfo* bootinfo)
	{
		// init the console
		console::init(bootinfo->fbHorz, bootinfo->fbVert, bootinfo->fbScanWidth);

		println("[nx] kernel has control");
		println("bootloader ident: '%c%c%c'\n", bootinfo->ident[0], bootinfo->ident[1], bootinfo->ident[2]);

		// setup all of our memory facilities.
		pmm::init(bootinfo);
		vmm::init(bootinfo);
		heap::init();

		// init the console, but more efficiently.
		console::init_stage2();

		// initialise the vfs so we can read the initrd
		vfs::init();

		// mount the tarfs at /initrd
		initrd::init(bootinfo);

		for(int i = 0; i < 20; i++)
			println("filler %d", i);
	}
}












extern "C" void kernel_main(nx::BootInfo* bootinfo)
{
	nx::kernel_main(bootinfo);


	nx::println("\nnothing to do...");

	while(true)
		;
}

















