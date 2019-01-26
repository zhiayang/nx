// main.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

#include "devices/x64/apic.h"
#include "devices/x64/pit8253.h"

namespace nx
{
	int64_t idle_thread()
	{
		while(true) asm volatile ("hlt");

		// how?!
		return 1;
	}

	int64_t work_thread()
	{

		return 0;
	}

	static void one()
	{
		util::printStackTrace();
	}

	static void two()
	{
		one();
	}

	static void three()
	{
		two();
	}

	static void four()
	{
		three();
	}

	static void five()
	{
		four();
	}

	void kernel_main(BootInfo* bootinfo)
	{
		// open all hatches
		// extend all flaps and drag fins

		// init the fallback console
		console::fallback::init(bootinfo->fbHorz, bootinfo->fbVert, bootinfo->fbScanWidth);

		// basically sets up the IDT so we can handle exceptions (instead of seemingly hanging)
		cpu::idt::init();
		exceptions::init();

		println("[nx] kernel has control");
		println("bootloader ident: '%c%c%c'\n", bootinfo->ident[0], bootinfo->ident[1], bootinfo->ident[2]);

		if(bootinfo->version < 1)
			abort("invalid bootloader version: %d; at least version %d is required!", bootinfo->version, 1);

		scheduler::setupKernelProcess(bootinfo->pml4Address);

		// setup all of our memory facilities.
		pmm::init(bootinfo);
		vmm::init(scheduler::getKernelProcess());
		heap::init();

		util::initSymbols(bootinfo);

		// setup the vfs and the initrd
		{
			vfs::init();
			initrd::init(bootinfo);
		}

		// init the real console
		console::init(bootinfo->fbHorz, bootinfo->fbVert, bootinfo->fbScanWidth);


		// basically sets up some datastructures. nothing much.
		scheduler::preinitCPUs();

		// read the acpi tables -- includes multiproc (MADT), timer (HPET)
		acpi::init(bootinfo);

		// initialise the interrupt controller -- either the IOAPIC/LAPIC, or the 8259 PIC (depending on the acpi tables)
		interrupts::init();
		interrupts::enable();

		{
			device::pit8253::enable(1);
			device::apic::setInterrupt(device::apic::getISAIRQMapping(0), 0, 0);
		}


		// hopefully we are flying more than half a ship at this point
		// setup an idle thread and a work thread.

		five();
	}
}












extern "C" void kernel_main(nx::BootInfo* bootinfo)
{
	nx::kernel_main(bootinfo);

	nx::println("\nnothing to do...");

	while(true)
		;
}

















