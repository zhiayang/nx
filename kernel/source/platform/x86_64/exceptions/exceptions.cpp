// exceptions.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"


namespace nx {
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

	struct IDTEntry
	{
		uint16_t    base_low;
		uint16_t    selector;
		uint8_t     ist_offset;
		uint8_t     flags;
		uint16_t    base_mid;
		uint32_t    base_high;
		uint32_t    always0;
	} __attribute__((packed));

	struct IDTPointer
	{
		uint16_t limit;
		uint64_t base;
	} __attribute__((packed));

	static constexpr uint8_t PIC1_CMD  = 0x20;
	static constexpr uint8_t PIC1_DATA = 0x21;
	static constexpr uint8_t PIC2_CMD  = 0xA0;
	static constexpr uint8_t PIC2_DATA = 0xA1;

	static constexpr int NumIDTEntries = 256;

	static constexpr const char* ExceptionMessages[] =
	{
		"division by zero",
		"debug",
		"non maskable interrupt",
		"breakpoint",
		"into detected overflow",
		"out of bounds",
		"invalid opcode",
		"no coprocessor",

		"double fault",
		"coprocessor segment overrun",
		"bad tss",
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



	static IDTEntry idt[NumIDTEntries];
	static IDTPointer idtp;

	extern "C" void nx_x64_loadidt(uint64_t);


	void setInterruptGate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags)
	{
		// The interrupt routine's base address
		idt[num].base_low   = (base & 0xFFFF);
		idt[num].base_mid   = (base >> 16) & 0xFFFF;
		idt[num].base_high  = (base >> 32) & 0xFFFFFFFF;

		// The segment or 'selector' that this IDT entry will use
		// is set here, along with any access flags

		idt[num].selector = sel;
		if(num < 32)    idt[num].ist_offset = 0x0;		// ????? should be 1 or something.
		else            idt[num].ist_offset = 0x0;

		idt[num].always0 = 0;
		idt[num].flags = flags;
	}

	extern "C" void EXC_Handler_0();
	extern "C" void EXC_Handler_1();
	extern "C" void EXC_Handler_2();
	extern "C" void EXC_Handler_3();
	extern "C" void EXC_Handler_4();
	extern "C" void EXC_Handler_5();
	extern "C" void EXC_Handler_6();
	extern "C" void EXC_Handler_7();
	extern "C" void EXC_Handler_8();
	extern "C" void EXC_Handler_9();
	extern "C" void EXC_Handler_10();
	extern "C" void EXC_Handler_11();
	extern "C" void EXC_Handler_12();
	extern "C" void EXC_Handler_13();
	extern "C" void EXC_Handler_14();
	extern "C" void EXC_Handler_15();
	extern "C" void EXC_Handler_16();
	extern "C" void EXC_Handler_17();
	extern "C" void EXC_Handler_18();
	extern "C" void EXC_Handler_19();
	extern "C" void EXC_Handler_20();
	extern "C" void EXC_Handler_21();
	extern "C" void EXC_Handler_22();
	extern "C" void EXC_Handler_23();
	extern "C" void EXC_Handler_24();
	extern "C" void EXC_Handler_25();
	extern "C" void EXC_Handler_26();
	extern "C" void EXC_Handler_27();
	extern "C" void EXC_Handler_28();
	extern "C" void EXC_Handler_29();
	extern "C" void EXC_Handler_30();
	extern "C" void EXC_Handler_31();
	extern "C" void EXC_Handler_32();


	void init(BootInfo*)
	{
		idtp.limit = (sizeof(IDTEntry) * NumIDTEntries) - 1;
		idtp.base = (uintptr_t) &idt;

		memset(&idt, 0, NumIDTEntries * sizeof(IDTEntry));

		setInterruptGate(0, (uint64_t) EXC_Handler_0, 0x08, 0x8E);
		setInterruptGate(1, (uint64_t) EXC_Handler_1, 0x08, 0x8E);
		setInterruptGate(2, (uint64_t) EXC_Handler_2, 0x08, 0x8E);
		setInterruptGate(3, (uint64_t) EXC_Handler_3, 0x08, 0x8E);
		setInterruptGate(4, (uint64_t) EXC_Handler_4, 0x08, 0x8E);
		setInterruptGate(5, (uint64_t) EXC_Handler_5, 0x08, 0x8E);
		setInterruptGate(6, (uint64_t) EXC_Handler_6, 0x08, 0x8E);
		setInterruptGate(7, (uint64_t) EXC_Handler_7, 0x08, 0x8E);

		setInterruptGate(8, (uint64_t) EXC_Handler_8, 0x08, 0x8E);
		setInterruptGate(9, (uint64_t) EXC_Handler_9, 0x08, 0x8E);
		setInterruptGate(10, (uint64_t) EXC_Handler_10, 0x08, 0x8E);
		setInterruptGate(11, (uint64_t) EXC_Handler_11, 0x08, 0x8E);
		setInterruptGate(12, (uint64_t) EXC_Handler_12, 0x08, 0x8E);
		setInterruptGate(13, (uint64_t) EXC_Handler_13, 0x08, 0x8E);
		setInterruptGate(14, (uint64_t) EXC_Handler_14, 0x08, 0x8E);
		setInterruptGate(15, (uint64_t) EXC_Handler_15, 0x08, 0x8E);

		setInterruptGate(16, (uint64_t) EXC_Handler_16, 0x08, 0x8E);
		setInterruptGate(17, (uint64_t) EXC_Handler_17, 0x08, 0x8E);
		setInterruptGate(18, (uint64_t) EXC_Handler_18, 0x08, 0x8E);
		setInterruptGate(19, (uint64_t) EXC_Handler_19, 0x08, 0x8E);
		setInterruptGate(20, (uint64_t) EXC_Handler_20, 0x08, 0x8E);
		setInterruptGate(21, (uint64_t) EXC_Handler_21, 0x08, 0x8E);
		setInterruptGate(22, (uint64_t) EXC_Handler_22, 0x08, 0x8E);
		setInterruptGate(23, (uint64_t) EXC_Handler_23, 0x08, 0x8E);

		setInterruptGate(24, (uint64_t) EXC_Handler_24, 0x08, 0x8E);
		setInterruptGate(25, (uint64_t) EXC_Handler_25, 0x08, 0x8E);
		setInterruptGate(26, (uint64_t) EXC_Handler_26, 0x08, 0x8E);
		setInterruptGate(27, (uint64_t) EXC_Handler_27, 0x08, 0x8E);
		setInterruptGate(28, (uint64_t) EXC_Handler_28, 0x08, 0x8E);
		setInterruptGate(29, (uint64_t) EXC_Handler_29, 0x08, 0x8E);
		setInterruptGate(30, (uint64_t) EXC_Handler_30, 0x08, 0x8E);
		setInterruptGate(31, (uint64_t) EXC_Handler_31, 0x08, 0x8E);


		// Remap the IRQs from 0 - 7 -> 8 - 15 to 32-47
		port::write1b(PIC1_CMD, 0x11);
		port::write1b(PIC2_CMD, 0x11);
		port::write1b(PIC1_DATA, 0x20);
		port::write1b(PIC2_DATA, 0x28);
		port::write1b(PIC1_DATA, 0x04);
		port::write1b(PIC2_DATA, 0x02);
		port::write1b(PIC1_DATA, 0x01);
		port::write1b(PIC2_DATA, 0x01);
		port::write1b(PIC1_DATA, 0x0);
		port::write1b(PIC2_DATA, 0x0);

		nx_x64_loadidt((uint64_t) &idtp);
		// println("exceptions handlers initialised");
	}



	static void dumpRegs(RegState_t* r)
	{
		println("rax:    %16.8lx   rbx:   %16.8lx", r->rax, r->rbx);
		println("rcx:    %16.8lx   rdx:   %16.8lx", r->rcx, r->rdx);
		println("r08:    %16.8lx   r09:   %16.8lx", r->r8, r->r9);
		println("r10:    %16.8lx   r11:   %16.8lx", r->r10, r->r11);
		println("r12:    %16.8lx   r13:   %16.8lx", r->r12, r->r13);
		println("r14:    %16.8lx   r15:   %16.8lx", r->r14, r->r15);
		println("rdi:    %16.8lx   rsi:   %16.8lx", r->rdi, r->rsi);
		println("rbp:    %16.8lx   rsp:   %16.8lx", r->rbp, r->rsp);
		println("rip:    %16.8lx   cs:    %16.8lx", r->rip, r->cs);
		println("ss:     %16.8lx   u-rsp: %16.8lx", r->ss, r->useresp);
		println("rflags: %16.8lx   cr2:   %16.8lx", r->rflags, r->cr2);
	}


	extern "C" void nx_x64_exception(RegState_t* regs)
	{
		uint64_t cr2;
		uint64_t cr3;

		asm volatile("mov %%cr2, %0" : "=r" (cr2));
		asm volatile("mov %%cr3, %0" : "=r" (cr3));

		// TODO: kill the process if this happens
		// TODO: also stuff like page faults aren't always bad news

		println("\n");
		println("cpu exception %d: %s", regs->InterruptID, ExceptionMessages[regs->InterruptID]);
		println("error code:   %d", regs->ErrorCode);

		dumpRegs(regs);

		if(regs->InterruptID == 14)
		{
			println("");

			// The error code gives us details of what happened.
			uint8_t notPresent          = !(regs->ErrorCode & 0x1); // Page not present
			uint8_t access              = regs->ErrorCode & 0x2;    // Write operation?
			uint8_t supervisor          = regs->ErrorCode & 0x4;    // Processor was in user-mode?
			uint8_t reservedBits        = regs->ErrorCode & 0x8;    // Overwritten CPU-reserved bits of page entry?
			uint8_t instructionFetch    = regs->ErrorCode & 0x10;   // Caused by an instruction fetch?

			println("cr3:    %p", cr3);
			print  ("reason:");

			if(notPresent)          print(" not present");
			if(access)              print(" read-only");
			if(supervisor)          print(" supervisor");
			if(reservedBits)        print(" reserved write");
			if(instructionFetch)    print(" instruction fetch");

			println("");
		}

		abort("unrecoverable cpu exception");
	}
}
}























