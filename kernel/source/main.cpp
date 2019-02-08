// main.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "cpu/cpuid.h"
#include "misc/params.h"

#include "loader.h"

#define NX_BOOTINFO_VERSION NX_SUPPORTED_BOOTINFO_VERSION
#include "bootinfo.h"




namespace nx
{
	[[noreturn]] static void idle_thread() { while(true) asm volatile ("hlt"); }

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

		println("*yawn*. goodbye!");
		scheduler::exit();
	}

	void kernel_main()
	{
		/*
			TODO

			extmm currently uses fixed addresses for the extent stack. vmm uses extmm.

			thus, we cannot allocate virtual memory from other address spaces, because the extmm will allocate from the current one
			(even if the extmm::State is different).

			it's definitely useful to be able to allocate memory from other address spaces (eg. when loading programs),
			so we want to keep that ability.

			the issue is, if we keep the currently simple design (and implementation) of the extmm, then it's quite difficult to get at
			the state of the other memory.


			potential solution:
			have an extmm2 that is basically extmm1, but instead of using a fixed-size array of extents, we make a linked-list of
			extents and allocate them on the kernel heap. this way, all extents from all address spaces will be available for us to
			get at, and we can just follow and free pointers when we cleanup the process.

			there's a slight issue with initialisation; the good thing about the current extmm is that it does not depend on the kernel
			heap, because the heap uses the vmm (which uses extmm) to allocate pages. if we change the extmm to use the heap, then
			we have a circular dependency here. not to mention the pmm also uses extmm...


			potential solution:
			{
				we can maintain the linked-list of extents, but we create the bootstrap extents contiguously in an array-like fashion,
				similar to how we currently manage them. for the vmm, we only create one extent per address space range anyway, so it's
				not like we need a lot of memory to do it.

				problem:
				although we bootstrap the pmm with one page worth of extent-space, after bootstrapping it can already start to allocate
				pages. thus it can extend itself when we start to 'deallocate' the memory-map entries (potentially adding lots of extents if
				the physical memory is fragmented).

				if we stick to one page of bootstrap, then if we have too many regions of physical memory, things might get ugly.
				we can mitigate this by allowing a variable number of pages to bootstrap -- by calculating how many memory map entries
				we have.

				note: we don't need a doubly-linked list of extents, but singly-linked makes the extent size 24 bytes, which doesn't divide
				nicely by the page size (giving 170.6666...)

				for the rest of the stuff, we should be good to go. see the trello for implementation steps.
			}
		*/








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
			println("succeeded = %d", success);
		}







		// auto worker1 = scheduler::createThread(scheduler::getKernelProcess(), work_thread1);
		// auto worker2 = scheduler::createThread(scheduler::getKernelProcess(), work_thread2);
		// scheduler::addThread(worker1);
		// scheduler::addThread(worker2);


		uint64_t ctr = 0;
		while(true)
		{
			if(++ctr % 7500000 == 0) print("q");
		}
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
			auto idle = scheduler::createThread(kernelProc, idle_thread);
			auto worker = scheduler::createThread(kernelProc, kernel_main);

			scheduler::init(idle, worker);

			// we are all done here: the worker thread will take care of the rest of kernel startup.
		}
	}
}



























