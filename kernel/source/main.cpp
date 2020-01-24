// main.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "misc/params.h"

#include "loader.h"

#define NX_BOOTINFO_VERSION NX_MAX_BOOTINFO_VERSION
#include "bootinfo.h"

#include "cpu/cpu.h"
#include "cpu/exceptions.h"
#include "devices/pc/apic.h"

namespace nx
{
	// TODO: jesus christ get rid of this
	static scheduler::Process* ps2driverproc = 0;
	void kernel_main()
	{
		log("kernel", "started main worker thread\n");

		// start the ipc dispatcher
		ipc::init();

		// start the irq dispatcher
		interrupts::init();

		scheduler::addThread(loader::loadProgram("/initrd/services/tty-svr"));
		auto placebo = scheduler::addThread(loader::loadProgram("/initrd/drivers/placebo"));


		// todo: make this more dynamic
		{
			auto thr = loader::loadProgram("/initrd/drivers/ps2");
			ps2driverproc = thr->parent;

			scheduler::allowProcessIOPort(thr->parent, 0x60);
			scheduler::allowProcessIOPort(thr->parent, 0x64);

			{
				int irq = device::ioapic::getISAIRQMapping(1);

				interrupts::unmaskIRQ(irq);
				device::ioapic::setIRQMapping(irq, /* vector */ 0x10, /* lapic id */ 0);

				interrupts::addIRQHandler(0x10, thr);
			}

			scheduler::addThread(thr);
		}


		scheduler::addThread(scheduler::createThread(scheduler::getKernelProcess(), []() {

			constexpr uint32_t colours[] = {
				0xff'ff0000,
				0xff'00ff00,
				0xff'0000ff,
			};

			size_t ctr = 0;
			while(true)
			{
				ctr++;
				uint32_t* fb = (uint32_t*) addrs::KERNEL_FRAMEBUFFER;
				for(int y = 0; y < 80; y++)
				{
					for(int x = 1440 - 160; x < 1440 - 80; x++)
					{
						*(fb + y * 1440 + x) = colours[ctr % 3];
					}
				}

				serial::debugprint("*");
				scheduler::sleep(time::milliseconds(100).ns());
			}
		}));


		log("kernel", "going to sleep...");
		scheduler::sleep(500'000'000);
		log("kernel", "woken from slumber, committing murder!");
		ipc::signalProcess(placebo->parent, ipc::SIGNAL_TERMINATE, ipc::signal_message_body_t(31, 45, 67));

		while(true)
			asm volatile("pause");
	}










	// ewwwwwwww
	uint32_t fbResX = 0;
	uint32_t fbResY = 0;
	uint32_t fbScan = 0;

	void init(BootInfo* bootinfo)
	{
		// open all hatches, extend all flaps and drag fins
		interrupts::disable();

		// init the fallback console
		console::fallback::init(bootinfo->fbHorz, bootinfo->fbVert, bootinfo->fbScanWidth);

		fbResX = bootinfo->fbHorz;
		fbResY = bootinfo->fbVert;
		fbScan = bootinfo->fbScanWidth;

		// basically sets up the IDT so we can handle exceptions (instead of seemingly hanging)
		cpu::idt::init();
		exceptions::init();

		println("[nx] kernel has control");
		println("bootloader ident: '%c%c%c', version: %d\n",
			bootinfo->ident[0], bootinfo->ident[1], bootinfo->ident[2], bootinfo->version);

		if(bootinfo->version < NX_MIN_BOOTINFO_VERSION)
		{
			abort("invalid bootloader version: %d; at least version %d is required!",
				bootinfo->version, NX_MIN_BOOTINFO_VERSION);
		}
		else if(bootinfo->version > NX_MAX_BOOTINFO_VERSION)
		{
			warn("kernel", "bootloader version %d is newer than supported version %d",
				bootinfo->version, NX_MAX_BOOTINFO_VERSION);
		}

		// set the wp bit so we don't go around writing stuff.
		cpu::writeCR0(cpu::readCR0() | cpu::CR0_WP);

		if(!cpu::didEnableNoExecute())
			warn("kernel", "cpu does not support no-execute bit");

		scheduler::setupKernelProcess(bootinfo->pml4Address);

		// setup all of our memory facilities.
		pmm::init(bootinfo);
		vmm::init(scheduler::getKernelProcess());

		heap::init();
		fixed_heap::init();

		// init cpuid so we can start detecting features.
		cpu::initCPUID();

		// setup the fpu -- including x87, sse, and avx
		cpu::fpu::init();

		// parse the kernel parameters from the bootloader.
		params::init(bootinfo);

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

		// find the symbol table & demangle symbols for backtracing purposes
		if(!params::haveOption("no_symbols"))
			util::initSymbols(bootinfo);

		// we should be done with the bootinfo now.
		pmm::freeAllEarlyMemory(bootinfo);

		// initialise the interrupt controller (APIC or PIC).
		// init_arch allows us to do basic scheduling. after the scheduler
		// is up, we must call interrupts::init.
		interrupts::init_arch();

		// hopefully we are flying more than half a ship at this point
		// initialise the scheduler with some threads -- this function will end!!
		{
			scheduler::bootstrap();
			scheduler::init();
			scheduler::installTickHandlers();

			syscall::init();

			scheduler::addThread(scheduler::createThread(scheduler::getKernelProcess(), kernel_main));
			scheduler::start();

			// we are all done here: the worker thread will take care of the rest of kernel startup.
		}
	}
}





















