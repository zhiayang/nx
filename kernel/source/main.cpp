// main.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

#include "devices/x64/apic.h"
#include "devices/x64/pit8253.h"

namespace nx
{
	[[noreturn]] static int64_t idle_thread() { while(true) asm volatile ("hlt"); }

	static mutex s;

	int64_t work_thread2()
	{
		s.lock();
		size_t ctr = 0;
		while(true)
		{
			if(++ctr % 5000000 == 0) print("2");

		}
		return 1;
	}


	int64_t work_thread1()
	{
		size_t ctr = 0;
		while(true)
		{
			if(++ctr % 5000000 == 0) print("1");
		}
		return 1;
	}


	int64_t kernel_main()
	{
		log("kernel", "started main worker thread\n");

		s = mutex();
		s.lock();

		auto worker1 = scheduler::createThread(scheduler::getKernelProcess(), work_thread1);
		auto worker2 = scheduler::createThread(scheduler::getKernelProcess(), work_thread2);
		scheduler::add(worker1);
		scheduler::add(worker2);


		uint64_t ctr = 0;
		while(true)
		{
			if(++ctr % 5000000 == 0) print("q");
			if(ctr %   1000000000 == 0) { print("!"); s.unlock(); }
		}

		// how?!
		return 1;
	}


















	void init(BootInfo* bootinfo)
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
		auto kernelProc = scheduler::getKernelProcess();

		// setup all of our memory facilities.
		pmm::init(bootinfo);
		vmm::init(kernelProc);
		heap::init();

		util::initSymbols(bootinfo);

		// setup the vfs and the initrd (for fonts)
		vfs::init();
		initrd::init(bootinfo);

		// init the real console
		console::init(bootinfo->fbHorz, bootinfo->fbVert, bootinfo->fbScanWidth);

		// basically sets up some datastructures. nothing much.
		scheduler::preinitCPUs();

		// read the acpi tables -- includes multiproc (MADT), timer (HPET)
		acpi::init(bootinfo);

		// initialise the interrupt controller -- either the IOAPIC/LAPIC, or the 8259 PIC (depending on the acpi tables)
		interrupts::init();
		interrupts::enable();


		// TODO: make this more robust?
		// eg: find appropriate timer device automatically (HPET/LAPIC/PIT)
		{
			int irq = device::apic::getISAIRQMapping(0);

			device::pit8253::enable();
			device::apic::setInterrupt(irq, 0, 0);
			scheduler::setTickIRQ(irq);
		}


		// hopefully we are flying more than half a ship at this point
		// initialise the scheduler with some threads -- this function will end!!
		{
			auto idle = scheduler::createThread(kernelProc, idle_thread);
			auto worker = scheduler::createThread(kernelProc, kernel_main);

			scheduler::init(idle, worker);

			// we are all done here: the worker thread will take care of the rest of kernel startup.
		}
	}
}



























