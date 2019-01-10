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

		// the original plan was to initialise the 'real' console and use ttf fonts and stb_truetype to render
		// a sweet looking terminal. unfortunately it looks like shit at small sizes even with SDF.
		// TODO: maybe this? https://github.com/chrisy/fontem
		// console::init(bootinfo->fbHorz, bootinfo->fbVert, bootinfo->fbScanWidth);
	}
}












extern "C" void kernel_main(nx::BootInfo* bootinfo)
{
	nx::kernel_main(bootinfo);


	nx::println("\nnothing to do...");

	while(true)
		;
}

















