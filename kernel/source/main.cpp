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

#include "rpc.h"

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

		// start the debug console
		// debugcon::init();


		log("mem", "phys: used {} kb / {} kb", pmm::getNumAllocatedBytes() / 1024,
			pmm::getTotalPhysicalMemory() / 1024);


		// scheduler::addThread(loader::loadProgram("/initrd/services/tty-svr"));
		scheduler::addThread(loader::loadProgram("/initrd/services/vfs-svr"));

		auto placebo = scheduler::addThread(loader::loadProgram("/initrd/drivers/placebo"));


		// todo: make this more dynamic
		{

			// BUG: sometimes (and quite frequently) the ps2 driver never gets to wake up.
			// no idea wtf is happening on that front.
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
				uint32_t* fb = (uint32_t*) addrs::KERNEL_FRAMEBUFFER.ptr();
				for(int y = 0; y < 80; y++)
					for(int x = 1440 - 160; x < 1440 - 80; x++)
						*(fb + y * 1440 + x) = colours[ctr % 3];

				scheduler::sleep(time::milliseconds(100));
			}
		}));

		log("kernel", "going to sleep...");
		scheduler::sleep(time::milliseconds(500));

		log("kernel", "woken from slumber, committing murder! {p}", (void*) placebo);
		ipc::signal("/proc/name/placebo", ipc::SIGNAL_TERMINATE, ipc::signal_message_body_t(31, 45, 67));

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
		scheduler::setupEarlyCPULocalState();

		interrupts::disable();
		serial::init();

		// init the fallback console
		console::fallback::init(bootinfo->fbHorz, bootinfo->fbVert, bootinfo->fbScanWidth);

		fbResX = bootinfo->fbHorz;
		fbResY = bootinfo->fbVert;
		fbScan = bootinfo->fbScanWidth;

		// basically sets up the IDT so we can handle exceptions (instead of seemingly hanging)
		cpu::idt::init();
		exceptions::init();

		println("[nx] kernel has control");
		println("bootloader ident: '{}{}{}', version: {}\n",
			bootinfo->ident[0], bootinfo->ident[1], bootinfo->ident[2], bootinfo->version);

		if(bootinfo->version < NX_MIN_BOOTINFO_VERSION)
		{
			abort("invalid bootloader version: {}; at least version {} is required!",
				bootinfo->version, NX_MIN_BOOTINFO_VERSION);
		}
		else if(bootinfo->version > NX_MAX_BOOTINFO_VERSION)
		{
			warn("kernel", "bootloader version {} is newer than supported version {}",
				bootinfo->version, NX_MAX_BOOTINFO_VERSION);
		}

		if(!cpu::didEnableNoExecute())
			warn("kernel", "cpu does not support no-execute bit");

		if(!cpu::supportsDoubleWordCompareExchange())
		{
			error("kernel", "cpu does not support cmpxchg16b");
			abort("hardware does not support required instruction!");
		}

		// set the wp bit so we don't go around writing stuff.
		cpu::writeCR0(cpu::readCR0() | cpu::CR0_WP);

		scheduler::setupKernelProcess(PhysAddr(bootinfo->pml4Address));

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

		// init the real console
		console::init(bootinfo->fbHorz, bootinfo->fbVert, bootinfo->fbScanWidth);

		// basically sets up some datastructures. nothing much.
		scheduler::preinitCPUs();

		// read the acpi tables -- includes multiproc (MADT), timer (HPET)
		if(!params::haveOption("no_acpi"))
			acpi::init(bootinfo);

		// find the symbol table & demangle symbols for backtracing purposes
		if(!params::haveOption("no_symbols"))
			util::initKernelSymbols(bootinfo);

		// initialise the interrupt controller (APIC or PIC).
		// init_arch allows us to do basic scheduling.
		interrupts::init_arch();

		// bootstrap the scheduler first
		scheduler::bootstrap();

		// setup the vfs and the initrd
		vfs::init();
		initrd::init(bootinfo);

		// we should be done with the bootinfo now.
		pmm::freeAllEarlyMemory(bootinfo);

		// hopefully we are flying more than half a ship at this point
		// initialise the scheduler with some threads -- this function will end!!
		{
			scheduler::init();
			scheduler::installTickHandlers();

			syscall::init();

			scheduler::addThread(scheduler::createThread(scheduler::getKernelProcess(), kernel_main));
			scheduler::start();

			// we are all done here: the worker thread will take care of the rest of kernel startup.
		}
	}
}




/*
	todo (24/09/2020):

	1. figure out how to implement 'rpc_forward' system call.
		we want the following flow to be possible: client -> server -> delegate -> client
		in the context of vfs, this means that the filesystem driver sends data straight back to the
		client process without going through the vfs on the way back.

		ideally, this will scale to the following scenario as well: client -> vfs -> fs -> disk -> client,
		so we do not need a round-trip back to the vfs nor the filesystem driver either. but that's for
		another time.

		currently, we removed the requirement that only the designated callee can call rpc_return on a
		connection; at the most basic level, this enables the delegate server to reply on the connection
		that it does not own. not sure if this is a good thing; we might want to add kernel mechanisms
		to ensure that only properly delegated servers (eg. rpc_delegate(conn, pid)) can reply to a given
		connection.

		anyway, the next thing is how to actually forward the message. currently the idea is that the
		server and the delegate will have their own connection, and rpc_forward will copy the message from
		the client<->server connection to the server<->delegate connection, potentially modifying some
		flags (eg. RPC_FORWARDED?). importantly, there needs to be a way to tell the delegate the original
		connection id that it should reply to.

		the more i think about it, the more i feel that rpc_forward should not actually be a thing... because
		it is obvious that each intermediate server (in a chain of delegates) will need to transform the data
		in *some* manner -- if not then it would be pretty useless. furthermore, we need a way to tell the
		delegate about the original connection, plus whatever data; it makes sense that we would just use
		rpc_call to invoke the delegate with application-specific parameter formats and set some application
		defined flag as well to indicate that it's a forwarded message...?

		this would imply that any given server must be written to accept forwarded messages and with a specific
		protocol at that, but i think that's fine? like we can write all the fs drivers to interpret forwards
		from the vfs server, and we can write all the disk drivers to interpret forwards from fs drivers...


	2. implement basic vfs proof of concept:
		(a) tarfs driver
		(b) vfs mount, tell it about tarfs somehow
*/


/*
	todo (09/01/2021):

	1. all prior todos

	2. a lot of the syscalls don't return error codes; we need to figure out a way to either
		(a) return an error code nicely to the user via the return value
		(b) use (clobber) another register to return an error code "out of band"
*/

/*
	todo (19/02/2021):

	1. all prior todos

	2. it would be nice to have a mechanism for "prefetching" pagefaults (we need that for mmap's
		MAP_POPULATE in the future anyway).
*/













