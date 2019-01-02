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

		struct foo { int a, b, c, d, e, f; };
		struct bar { int x[64], y[32]; };

		int num_foos = 0;
		int num_bars = 0;

		foo** foos = new foo*[10000 / 2];
		bar** bars = new bar*[10000 / 2];

		for(int i = 0; i < 10000; i++)
		{
			print("%d ", i);
			if(i % 2 == 0)  foos[num_foos] = new foo(), num_foos++;
			else            bars[num_bars] = new bar(), num_bars++;
		}

		for(int i = 0; i < num_foos; i++) delete foos[i];
		for(int i = 0; i < num_bars; i++) delete bars[i];

		delete[] foos;
		delete[] bars;

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

















