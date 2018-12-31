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

		// tell the world (kinda)
		serial::debugprint("[nx] kernel has control\n");


		serial::debugprint("initialising pmm");
		pmm::init(bootinfo->mmEntries, bootinfo->mmEntryCount);
	}
}



















extern "C" void kernel_main(nx::BootInfo* bootinfo)
{
	nx::kernel_main(bootinfo);

	while(true)
		;
}