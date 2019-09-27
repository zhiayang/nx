// exceptions.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "bfx.h"

namespace bfx {
namespace exceptions
{
	// note! corresponds to the order (in reverse) in handlers.s
	struct RegState_t
	{
		uint64_t cr2;
		uint64_t rsp;
		uint64_t rdi, rsi, rbp;
		uint64_t rax, rbx, rcx, rdx, r8, r9, r10, r11, r12, r13, r14, r15;

		uint64_t InterruptID, ErrorCode;
		uint64_t rip, cs, rflags, useresp, ss;
	};

	constexpr const char* ExceptionMessages[] =
	{
		"division by zero",
		"debug",
		"non maskable interrupt",
		"breakpoint",
		"into detected overflow",
		"out of bounds",
		"invalid opcode",
		"device not available",

		"double fault",
		"coprocessor segment overrun",
		"invalid tss",
		"segment not present",
		"stack fault",
		"general protection fault",
		"page fault",
		"reserved",

		"x87 fpu fault",
		"alignment check",
		"machine check",
		"sse fault",
		"reserved",
		"reserved",
		"reserved",
		"reserved",

		"reserved",
		"reserved",
		"reserved",
		"reserved",
		"reserved",
		"reserved",
		"reserved",
		"reserved"
	};



	extern "C" void nx_x64_exception_handler_0();
	extern "C" void nx_x64_exception_handler_1();
	extern "C" void nx_x64_exception_handler_2();
	extern "C" void nx_x64_exception_handler_3();
	extern "C" void nx_x64_exception_handler_4();
	extern "C" void nx_x64_exception_handler_5();
	extern "C" void nx_x64_exception_handler_6();
	extern "C" void nx_x64_exception_handler_7();
	extern "C" void nx_x64_exception_handler_8();
	extern "C" void nx_x64_exception_handler_9();
	extern "C" void nx_x64_exception_handler_10();
	extern "C" void nx_x64_exception_handler_11();
	extern "C" void nx_x64_exception_handler_12();
	extern "C" void nx_x64_exception_handler_13();
	extern "C" void nx_x64_exception_handler_14();
	extern "C" void nx_x64_exception_handler_15();
	extern "C" void nx_x64_exception_handler_16();
	extern "C" void nx_x64_exception_handler_17();
	extern "C" void nx_x64_exception_handler_18();
	extern "C" void nx_x64_exception_handler_19();
	extern "C" void nx_x64_exception_handler_20();
	extern "C" void nx_x64_exception_handler_21();
	extern "C" void nx_x64_exception_handler_22();
	extern "C" void nx_x64_exception_handler_23();
	extern "C" void nx_x64_exception_handler_24();
	extern "C" void nx_x64_exception_handler_25();
	extern "C" void nx_x64_exception_handler_26();
	extern "C" void nx_x64_exception_handler_27();
	extern "C" void nx_x64_exception_handler_28();
	extern "C" void nx_x64_exception_handler_29();
	extern "C" void nx_x64_exception_handler_30();
	extern "C" void nx_x64_exception_handler_31();

	constexpr void (*exception_handlers[32])() = {
		nx_x64_exception_handler_0,  nx_x64_exception_handler_1,  nx_x64_exception_handler_2,  nx_x64_exception_handler_3,
		nx_x64_exception_handler_4,  nx_x64_exception_handler_5,  nx_x64_exception_handler_6,  nx_x64_exception_handler_7,
		nx_x64_exception_handler_8,  nx_x64_exception_handler_9,  nx_x64_exception_handler_10, nx_x64_exception_handler_11,
		nx_x64_exception_handler_12, nx_x64_exception_handler_13, nx_x64_exception_handler_14, nx_x64_exception_handler_15,
		nx_x64_exception_handler_16, nx_x64_exception_handler_17, nx_x64_exception_handler_18, nx_x64_exception_handler_19,
		nx_x64_exception_handler_20, nx_x64_exception_handler_21, nx_x64_exception_handler_22, nx_x64_exception_handler_23,
		nx_x64_exception_handler_24, nx_x64_exception_handler_25, nx_x64_exception_handler_26, nx_x64_exception_handler_27,
		nx_x64_exception_handler_28, nx_x64_exception_handler_29, nx_x64_exception_handler_30, nx_x64_exception_handler_31,
	};


	void init()
	{
		for(int i = 0; i < 32; i++)
		{
			// 10x-er method of doing things, obviously.
			idt::setEntry(i, (uint64_t) exception_handlers[i],
				/* cs: */ 0x08, /* isRing3: */ false, /* nestable: */ false);
		}

		// note: on x86, we do not remap the IRQs on the PIC first,
		// because we 'prefer' to use the APIC system. when we call
		// interrupts::init(), we try to use the APIC unless it does not
		// exist; if it doesn't, then we call pic8259::init(), which does
		// the IRQ remapping. if not, we just mask all interrupts from the PIC.
	}



	static void dumpRegs(RegState_t* r)
	{
		using namespace serial;

		print("\n");
		print("rax:      %16.8lx   rbx:      %16.8lx\n", r->rax, r->rbx);
		print("rcx:      %16.8lx   rdx:      %16.8lx\n", r->rcx, r->rdx);
		print("r08:      %16.8lx   r09:      %16.8lx\n", r->r8, r->r9);
		print("r10:      %16.8lx   r11:      %16.8lx\n", r->r10, r->r11);
		print("r12:      %16.8lx   r13:      %16.8lx\n", r->r12, r->r13);
		print("r14:      %16.8lx   r15:      %16.8lx\n", r->r14, r->r15);
		print("rdi:      %16.8lx   rsi:      %16.8lx\n", r->rdi, r->rsi);
		print("rbp:      %16.8lx   rsp:      %16.8lx\n", r->rbp, r->rsp);
		print("rip:      %16.8lx   cs:       %16.8lx\n", r->rip, r->cs);
		print("ss:       %16.8lx   u-rsp:    %16.8lx\n", r->ss, r->useresp);
		print("rflags:   %16.8lx   cr2:      %16.8lx\n", r->rflags, r->cr2);
		print("\n");
	}


	static int faultCount = 0;
	extern "C" void nx_x64_handle_exception(RegState_t* regs)
	{
		using namespace serial;

		faultCount += 1;
		if(faultCount == 3)
		{
			print("\n\ntriple fault!!!\n");
			while(1);
		}

		uint64_t cr2;
		uint64_t cr3;

		asm volatile("mov %%cr2, %0" : "=r" (cr2));
		asm volatile("mov %%cr3, %0" : "=r" (cr3));

		// TODO: kill the process if this happens
		// TODO: also stuff like page faults aren't always bad news

		print("\n\n");
		print("cpu exception %d: %s\n", regs->InterruptID, ExceptionMessages[regs->InterruptID]);
		print("error code:   %d\n", regs->ErrorCode);


		if(regs->InterruptID == 13 && regs->ErrorCode != 0)
		{
			auto err = regs->ErrorCode;

			print("gpf:");

			if((err & 0x1))         print(" (external)");
			if((err & 0x6) == 0)    print(" (gdt)");
			if((err & 0x6) == 2)    print(" (idt)");
			if((err & 0x6) == 4)    print(" (ldt)");
			if((err & 0x6) == 6)    print(" (idt)");

			print(", selector: %x\n", err & 0xfff8);
		}

		dumpRegs(regs);

		if(regs->InterruptID == 14)
		{
			print("\n");

			// The error code gives us details of what happened.
			uint8_t notPresent          = !(regs->ErrorCode & 0x1); // Page not present
			uint8_t access              = regs->ErrorCode & 0x2;    // Write operation?
			uint8_t supervisor          = regs->ErrorCode & 0x4;    // Processor was in user-mode?
			uint8_t reservedBits        = regs->ErrorCode & 0x8;    // Overwritten CPU-reserved bits of page entry?
			uint8_t instructionFetch    = regs->ErrorCode & 0x10;   // Caused by an instruction fetch?

			print("cr3:    %p\n", cr3);
			print("addr:   %p\n", cr2);
			print("reason:");

			if(notPresent)          print(" (not present)");
			if(access)              print(" (read-only)");
			if(supervisor)          print(" (supervisor)");
			if(reservedBits)        print(" (reserved write)");
			if(instructionFetch)    print(" (instruction fetch)");

			print("\n");
		}

		print("\nfault location:\n");
		print("%p\n\n", regs->rip);

		while(1);
	}
}
}























