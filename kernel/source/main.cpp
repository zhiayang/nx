// main.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

#include "devices/x64/apic.h"
#include "devices/x64/pit8253.h"

namespace nx
{
	void kernel_main(BootInfo* bootinfo)
	{
		// open all hatches
		// extend all flaps and drag fins


		// init the console
		console::init(bootinfo->fbHorz, bootinfo->fbVert, bootinfo->fbScanWidth);

		// basically sets up the IDT so we can print stuff when we fault
		// (instead of seemingly hanging)
		cpu::idt::init();
		exceptions::init();

		println("[nx] kernel has control");
		println("bootloader ident: '%c%c%c'\n", bootinfo->ident[0], bootinfo->ident[1], bootinfo->ident[2]);

		// setup all of our memory facilities.
		pmm::init(bootinfo);
		vmm::init(bootinfo);
		heap::init();

		// init the console, but more efficiently.
		console::init_stage2();

		// basically sets up some datastructures. nothing much.
		scheduler::preinitProcs();

		// read the acpi tables -- includes multiproc (MADT), timer (HPET), interrupts (LAPIC & IOAPIC)
		acpi::init(bootinfo);

		// start doing interrupt stuff
		interrupts::init();
		interrupts::enable();

		{
			device::pit8253::enable(1);

			device::apic::setInterrupt(2, 32, 0);
			interrupts::unmaskIRQ(2);
		}


		// initialise the vfs so we can read the initrd
		vfs::init();

		// mount the tarfs at /initrd
		initrd::init(bootinfo);



		// hopefully we are flying more than half a ship at this point
	}
}












extern "C" void kernel_main(nx::BootInfo* bootinfo)
{
	nx::kernel_main(bootinfo);

	nx::println("\nnothing to do...");

	while(true)
		;
}

















