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