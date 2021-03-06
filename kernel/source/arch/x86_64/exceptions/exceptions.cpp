// exceptions.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace exceptions
{
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

	extern "C" void nx_x64_pagefault_handler();


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
			cpu::idt::setEntry(i, (uint64_t) exception_handlers[i],
				/* cs: */ 0x08, /* isRing3: */ false, /* nestable: */ false);
		}

		// note: on x86, we do not remap the IRQs on the PIC first,
		// because we 'prefer' to use the APIC system. when we call
		// interrupts::init(), we try to use the APIC unless it does not
		// exist; if it doesn't, then we call pic8259::init(), which does
		// the IRQ remapping. if not, we just mask all interrupts from the PIC.
	}

	extern "C" uint64_t __last_saved_error_code;

	static int faultCount = 0;
	extern "C" void nx_x64_handle_exception(cpu::ExceptionState* regs)
	{
		using namespace serial;

		faultCount += 1;
		if(faultCount == 3)
		{
			debugprintf("\n\ntriple fault!!!\n");
			while(1);
		}

		uint64_t cr2 = 0;
		uint64_t cr3 = 0;
		auto gsbase = cpu::readMSR(cpu::MSR_GS_BASE);

		asm volatile("mov %%cr2, %0" : "=r" (cr2));
		asm volatile("mov %%cr3, %0" : "=r" (cr3));

		debugprintf("\n\n");
		debugprintf("cpu exception {}: {}\n", regs->InterruptID, ExceptionMessages[regs->InterruptID]);
		debugprintf("error code:   {}  (last_saved_error: {})\n", regs->ErrorCode, __last_saved_error_code);

		// note: we must check gs, in case it was some kind of swapgs failure -- we don't want to end up triple-faulting.
		if(gsbase != 0 && scheduler::getInitPhase() >= scheduler::SchedulerInitPhase::SchedulerStarted)
		{
			debugprintf("pid: {} / tid: {}\n", scheduler::getCurrentProcess()->processId,
				scheduler::getCurrentThread()->threadId);
		}
		else
		{
			debugprintf("pre-scheduler\n");
		}

		if(regs->InterruptID == 13 && regs->ErrorCode != 0)
		{
			auto err = regs->ErrorCode;

			debugprintf("gpf:");

			if((err & 0x1))         debugprintf(" (external)");
			if((err & 0x6) == 0)    debugprintf(" (gdt)");
			if((err & 0x6) == 2)    debugprintf(" (idt)");
			if((err & 0x6) == 4)    debugprintf(" (ldt)");
			if((err & 0x6) == 6)    debugprintf(" (idt)");

			debugprintf(", selector: {x}\n", err & 0xfff8);
		}

		regs->dump();
		debugprintf("cr3:      {16.8x}\n", cr3);

		if(regs->InterruptID == 14)
		{
			debugprintf("\n");

			// The error code gives us details of what happened.
			uint8_t notPresent          = !(regs->ErrorCode & 0x1); // Page not present
			uint8_t access              = regs->ErrorCode & 0x2;    // Write operation?
			uint8_t supervisor          = regs->ErrorCode & 0x4;    // Processor was in user-mode?
			uint8_t reservedBits        = regs->ErrorCode & 0x8;    // Overwritten CPU-reserved bits of page entry?
			uint8_t instructionFetch    = regs->ErrorCode & 0x10;   // Caused by an instruction fetch?

			debugprintf("addr:   {p}\n", cr2);
			debugprintf("reason:");

			if(notPresent)          debugprintf(" (not present)");
			if(access)              debugprintf(" (read-only)");
			if(supervisor)          debugprintf(" (supervisor)");
			if(reservedBits)        debugprintf(" (reserved write)");
			if(instructionFetch)    debugprintf(" (instruction fetch)");

			debugprintf("\n");
		}

		debugprintf("\nfault location:\n");
		debugprintf("  {8x}  |  {}\n\n", regs->rip, syms::symbolicate(regs->rip, scheduler::getCurrentProcess()));

		// if this was a user program, then fuck that guy.
		if(gsbase != 0 && scheduler::getCurrentProcess() != scheduler::getKernelProcess())
		{
			util::printStackTrace(regs->rbp);

			// be a little smart about this. don't try to disasm if we died from an instruction fetch.
			if(regs->InterruptID != 14 || (regs->ErrorCode & 0x10) == 0)
				disasm::printDisassembly(regs->rip);

			faultCount -= 1;

			// we cannot exit "normally", we need to manually switch to another thread and reset our interrupt
			// nesting level, and do an IRET.
			scheduler::crashFromException();
		}
		else
		{
			// this is likely to cause faults if we did something really bad
			// so if we already faulted, don't try again.
			if(faultCount == 1)
			{
				util::printStackTrace(regs->rbp);
				disasm::printDisassembly(regs->rip);
			}

			debugprintf("\n!! error: unrecoverable cpu exception !!\n");
		}

		while(1);
	}



	// after we have ISTs, this is necessary.
	void initWithISTs()
	{
		for(int i = 0; i < 32; i++)
		{
			int ist = 0;

			if(i == 2 || i == 18)   ist = 7;    // NMI, MCE
			else if(i == 8)         ist = 6;    // DF
			else                    ist = 1;    // the rest

			cpu::idt::setEntry(i, (uint64_t) exception_handlers[i], /* cs: */ 0x08,
				/* isRing3: */ false, /* nestable: */ false, ist);
		}
	}


	extern "C" void nx_x64_handle_page_fault_internal(cpu::ExceptionState* regs)
	{
		// check if we handle it without crashing:
		if(vmm::handlePageFault(regs->cr2, regs->ErrorCode, regs->rip))
			return;

		// we couldn't, lmao -- just crash normally.
		nx_x64_handle_exception(regs);
	}

	void setupPageFaultHandler()
	{
		// set entry 14.
		cpu::idt::setEntry(14, (uint64_t) &nx_x64_pagefault_handler,
			/* cs: */ 0x08, /* isRing3: */ false, /* nestable: */ false);
	}
}

namespace cpu
{
	void ExceptionState::dump() const
	{
		auto r = this;
		using namespace serial;

		debugprintf("\n");
		debugprintf("rax:      {16.8x}   rbx:      {16.8x}\n", r->rax, r->rbx);
		debugprintf("rcx:      {16.8x}   rdx:      {16.8x}\n", r->rcx, r->rdx);
		debugprintf("r08:      {16.8x}   r09:      {16.8x}\n", r->r8, r->r9);
		debugprintf("r10:      {16.8x}   r11:      {16.8x}\n", r->r10, r->r11);
		debugprintf("r12:      {16.8x}   r13:      {16.8x}\n", r->r12, r->r13);
		debugprintf("r14:      {16.8x}   r15:      {16.8x}\n", r->r14, r->r15);
		debugprintf("rdi:      {16.8x}   rsi:      {16.8x}\n", r->rdi, r->rsi);
		debugprintf("rbp:      {16.8x}   rsp:      {16.8x}\n", r->rbp, r->rsp);
		debugprintf("rip:      {16.8x}   cs:       {16.8x}\n", r->rip, r->cs);
		debugprintf("ss:       {16.8x}   u-rsp:    {16.8x}\n", r->ss, r->useresp);
		debugprintf("rflags:   {16.8x}   cr2:      {16.8x}\n", r->rflags, r->cr2);
		debugprintf("\n");

		auto fsbase = cpu::readMSR(cpu::MSR_FS_BASE);
		auto gsbase = cpu::readMSR(cpu::MSR_GS_BASE);
		auto kgsbase = cpu::readMSR(cpu::MSR_KERNEL_GS_BASE);

		debugprintf("gs_base:  {16.8x}   kgs_base: {16.8x}\n", gsbase, kgsbase);
		debugprintf("fs_base:  {16.8x}\n", fsbase);
	}

	void InterruptedState::dump() const
	{
		auto r = this;

		using namespace serial;

		debugprintf("rax:      {16.8x}   rbx:      {16.8x}\n", r->rax, r->rbx);
		debugprintf("rcx:      {16.8x}   rdx:      {16.8x}\n", r->rcx, r->rdx);
		debugprintf("r08:      {16.8x}   r09:      {16.8x}\n", r->r8, r->r9);
		debugprintf("r10:      {16.8x}   r11:      {16.8x}\n", r->r10, r->r11);
		debugprintf("r12:      {16.8x}   r13:      {16.8x}\n", r->r12, r->r13);
		debugprintf("r14:      {16.8x}   r15:      {16.8x}\n", r->r14, r->r15);
		debugprintf("rdi:      {16.8x}   rsi:      {16.8x}\n", r->rdi, r->rsi);
		debugprintf("rbp:      {16.8x}   rsp:      {16.8x}\n", r->rbp, r->rsp);
		debugprintf("rip:      {16.8x}   cs:       {16.8x}\n", r->rip, r->cs);
		debugprintf("ss:       {16.8x}   rflags:   {16.8x}\n", r->ss, r->rflags);
	}
}
}























