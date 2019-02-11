// main.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "misc/params.h"

#include "loader.h"

#define NX_BOOTINFO_VERSION NX_SUPPORTED_BOOTINFO_VERSION
#include "bootinfo.h"




namespace nx
{
	void work_thread1()
	{
		size_t ctr = 0;
		while(true)
		{
			if(++ctr % 7500000 == 0) print("1");
		}
	}

	void work_thread2()
	{
		scheduler::sleep(time::milliseconds(500).ns());

		println("preparing to sleep for 2000 ms...");
		scheduler::sleep(time::milliseconds(2000).ns());

		println("*yawn*");

		uint64_t ctr = 0;
		while(true) if(++ctr % 5000000 == 0) print("x");
	}

	void kernel_main()
	{
		log("kernel", "started main worker thread\n");

		{
			auto f = vfs::open("/initrd/drivers/placebo", vfs::Mode::Read);
			assert(f);

			auto sz = vfs::stat(f).fileSize;
			auto buf = new uint8_t[sz];

			auto didRead = vfs::read(f, buf, sz);
			if(didRead != sz) abort("size mismatch! %zu / %zu", didRead, sz);

			auto proc = scheduler::createProcess("placebo", scheduler::Process::PROC_USER);

			addr_t entryPt = 0;
			bool success = loader::loadIndeterminateBinary(proc, buf, sz, &entryPt);
			if(success)
			{
				auto thr = scheduler::createThread(proc, (scheduler::Fn0Args_t) entryPt);
				scheduler::addThread(thr);
			}
			else
			{
				abort("failed to load!");
			}
		}


		auto worker2 = scheduler::createThread(scheduler::getKernelProcess(), work_thread2);
		scheduler::addThread(worker2);


		uint64_t ctr = 0;
		while(true) if(++ctr % 5000000 == 0) print("q");
	}


















	void init(BootInfo* bootinfo)
	{
		// open all hatches, extend all flaps and drag fins

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

		if(!cpu::didEnableNoExecute())
			warn("kernel", "cpu does not support no-execute bit");


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

		// initialise the interrupt controller (APIC or PIC)
		interrupts::init();
		interrupts::enable();

		// hopefully we are flying more than half a ship at this point
		// initialise the scheduler with some threads -- this function will end!!
		{
			scheduler::bootstrap();
			scheduler::init();
			scheduler::installTickHandlers();

			syscall::init();

			scheduler::addThread(scheduler::createThread(kernelProc, kernel_main));
			scheduler::start();

			// we are all done here: the worker thread will take care of the rest of kernel startup.
		}
	}
}



























