// interrupts.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "devices/x64/pic8259.h"

extern "C" void IRQ_Handler_0();
extern "C" void IRQ_Handler_1();
extern "C" void IRQ_Handler_2();
extern "C" void IRQ_Handler_3();
extern "C" void IRQ_Handler_4();
extern "C" void IRQ_Handler_5();
extern "C" void IRQ_Handler_6();
extern "C" void IRQ_Handler_7();
extern "C" void IRQ_Handler_8();
extern "C" void IRQ_Handler_9();
extern "C" void IRQ_Handler_10();
extern "C" void IRQ_Handler_11();
extern "C" void IRQ_Handler_12();
extern "C" void IRQ_Handler_13();
extern "C" void IRQ_Handler_14();
extern "C" void IRQ_Handler_15();

namespace nx {
namespace interrupts
{
	static bool IsAPICPresent = false;

	void init()
	{
		// TODO: probably do different stuff on different platforms
		if(IsAPICPresent = device::apic::init(); IsAPICPresent)
		{
			// disable the legacy PIC by masking all interrupts.
			device::pic8259::disable();
		}
		else
		{
			warn("apic", "system does not have apic; falling back to 8259 PIC");
			device::pic8259::init();
		}

		cpu::idt::setEntry(0x20 + 0,    (addr_t) IRQ_Handler_0,     0x08, 0x8E);
		cpu::idt::setEntry(0x20 + 1,    (addr_t) IRQ_Handler_1,     0x08, 0x8E);
		cpu::idt::setEntry(0x20 + 2,    (addr_t) IRQ_Handler_2,     0x08, 0x8E);
		cpu::idt::setEntry(0x20 + 3,    (addr_t) IRQ_Handler_3,     0x08, 0x8E);
		cpu::idt::setEntry(0x20 + 4,    (addr_t) IRQ_Handler_4,     0x08, 0x8E);
		cpu::idt::setEntry(0x20 + 5,    (addr_t) IRQ_Handler_5,     0x08, 0x8E);
		cpu::idt::setEntry(0x20 + 6,    (addr_t) IRQ_Handler_6,     0x08, 0x8E);
		cpu::idt::setEntry(0x20 + 7,    (addr_t) IRQ_Handler_7,     0x08, 0x8E);
		cpu::idt::setEntry(0x20 + 8,    (addr_t) IRQ_Handler_8,     0x08, 0x8E);
		cpu::idt::setEntry(0x20 + 9,    (addr_t) IRQ_Handler_9,     0x08, 0x8E);
		cpu::idt::setEntry(0x20 + 10,   (addr_t) IRQ_Handler_10,    0x08, 0x8E);
		cpu::idt::setEntry(0x20 + 11,   (addr_t) IRQ_Handler_11,    0x08, 0x8E);
		cpu::idt::setEntry(0x20 + 12,   (addr_t) IRQ_Handler_12,    0x08, 0x8E);
		cpu::idt::setEntry(0x20 + 13,   (addr_t) IRQ_Handler_13,    0x08, 0x8E);
		cpu::idt::setEntry(0x20 + 14,   (addr_t) IRQ_Handler_14,    0x08, 0x8E);
		cpu::idt::setEntry(0x20 + 15,   (addr_t) IRQ_Handler_15,    0x08, 0x8E);
	}

	void enable()
	{
		asm volatile("sti");
	}

	void disable()
	{
		asm volatile("cli");
	}



	//! ACHTUNG !
	//* note! we expect the interrupt controller to handle the irq -> idt translation by itself!
	//* meaning to say 'num' here will be 0-based -- it is the IRQ number after all, not the interrupt number.
	void maskIRQ(int num)
	{
		if(IsAPICPresent)   device::apic::maskIRQ(num);
		else                device::pic8259::maskIRQ(num);
	}

	void unmaskIRQ(int num)
	{
		if(IsAPICPresent)   device::apic::unmaskIRQ(num);
		else                device::pic8259::unmaskIRQ(num);
	}


	static int x = 0;
	extern "C" void nx_x64_interrupt(int num)
	{
		if(x++ == 40)
			x = 0, println("int %d", num);

		// send the eoi.
		if(IsAPICPresent)   device::apic::sendEOI(num);
		else                device::pic8259::sendEOI(num);
	}
}
}




























