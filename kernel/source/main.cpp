// main.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "misc/params.h"

#define NX_BOOTINFO_VERSION NX_SUPPORTED_BOOTINFO_VERSION
#include "bootinfo.h"




namespace nx
{
	[[noreturn]] static int64_t idle_thread() { while(true) asm volatile ("hlt"); }

	int64_t work_thread2()
	{
		// s.lock();
		size_t ctr = 0;
		while(true)
		{
			if(++ctr % 5000000 == 0) print("2");

		}
		return 1;
	}

	int64_t work_thread1()
	{
		scheduler::sleep(time::milliseconds(1000).ns());

		println("preparing to sleep for 5000 ms...");
		scheduler::sleep(time::milliseconds(5000).ns());

		println("*yawn*");

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

		auto worker1 = scheduler::createThread(scheduler::getKernelProcess(), work_thread1);
		auto worker2 = scheduler::createThread(scheduler::getKernelProcess(), work_thread2);
		scheduler::add(worker1);
		scheduler::add(worker2);


		uint64_t ctr = 0;
		while(true)
		{
			if(++ctr % 5000000 == 0) print("q");
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
		println("bootloader ident: '%c%c%c', version: %d\n",
			bootinfo->ident[0], bootinfo->ident[1], bootinfo->ident[2], bootinfo->version);

		if(bootinfo->version < NX_SUPPORTED_BOOTINFO_VERSION)
		{
			abort("invalid bootloader version: %d; at least version %d is required!",
				bootinfo->version, NX_SUPPORTED_BOOTINFO_VERSION);
		}
		else if(bootinfo->version > NX_SUPPORTED_BOOTINFO_VERSION)
		{
			warn("kernel", "bootloader version %d is newer than supported version %d",
				bootinfo->version, NX_SUPPORTED_BOOTINFO_VERSION);
		}

		scheduler::setupKernelProcess(bootinfo->pml4Address);
		auto kernelProc = scheduler::getKernelProcess();

		// setup all of our memory facilities.
		pmm::init(bootinfo);
		vmm::init(kernelProc);
		heap::init();

		// parse the kernel parameters from the bootloader.
		params::init(bootinfo);

		// find the symbol table & demangle symbols for backtracing purposes
		if(!params::haveOption("no_symbols"))
			util::initSymbols(bootinfo);

		// setup the vfs and the initrd (for fonts)
		vfs::init();
		initrd::init(bootinfo);

		// init the real console
		console::init(bootinfo->fbHorz, bootinfo->fbVert, bootinfo->fbScanWidth);

		// basically sets up some datastructures. nothing much.
		scheduler::preinitCPUs();

		// read the acpi tables -- includes multiproc (MADT), timer (HPET)
		if(!params::haveOption("no_acpi"))
			acpi::init(bootinfo);

		// we should be done with the bootinfo now.
		pmm::freeAllEarlyMemory(bootinfo);

		// initialise the interrupt controller (IOAPIC/LAPIC)
		interrupts::init();
		interrupts::enable();

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



























