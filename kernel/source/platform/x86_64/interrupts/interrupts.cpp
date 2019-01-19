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
		IsAPICPresent = device::apic::init();
		if(!IsAPICPresent)
		{
			warn("apic", "system does not have apic; falling back to 8259 PIC");
			device::pic8259::init();

			setIDTEntry(32, (addr_t) IRQ_Handler_0, 0x08, 0x8E);
			setIDTEntry(33, (addr_t) IRQ_Handler_1, 0x08, 0x8E);
			setIDTEntry(34, (addr_t) IRQ_Handler_2, 0x08, 0x8E);
			setIDTEntry(35, (addr_t) IRQ_Handler_3, 0x08, 0x8E);
			setIDTEntry(36, (addr_t) IRQ_Handler_4, 0x08, 0x8E);
			setIDTEntry(37, (addr_t) IRQ_Handler_5, 0x08, 0x8E);
			setIDTEntry(38, (addr_t) IRQ_Handler_6, 0x08, 0x8E);
			setIDTEntry(39, (addr_t) IRQ_Handler_7, 0x08, 0x8E);
			setIDTEntry(40, (addr_t) IRQ_Handler_8, 0x08, 0x8E);
			setIDTEntry(41, (addr_t) IRQ_Handler_9, 0x08, 0x8E);
			setIDTEntry(42, (addr_t) IRQ_Handler_10, 0x08, 0x8E);
			setIDTEntry(43, (addr_t) IRQ_Handler_11, 0x08, 0x8E);
			setIDTEntry(44, (addr_t) IRQ_Handler_12, 0x08, 0x8E);
			setIDTEntry(45, (addr_t) IRQ_Handler_13, 0x08, 0x8E);
			setIDTEntry(46, (addr_t) IRQ_Handler_14, 0x08, 0x8E);
			setIDTEntry(47, (addr_t) IRQ_Handler_15, 0x08, 0x8E);
		}
		else
		{
			// disable the legacy PIC by masking all interrupts.
			device::pic8259::disable();
		}
	}

	void enable()
	{
		asm volatile("sti");
	}

	void disable()
	{
		asm volatile("cli");
	}

	void setIDTEntry(uint8_t intr, addr_t fn, uint8_t codeSegment, uint8_t flags)
	{
		exceptions::setInterruptGate(intr, fn, codeSegment, flags);
		exceptions::enableGate(intr);
	}

	void clearIDTEntry(uint8_t intr)
	{
		exceptions::disableGate(intr);
	}




	static int x = 0;
	extern "C" void nx_x64_interrupt(int num)
	{
		if(x++ == 20)
			x = 0, println("int %d", num);

		// send the eoi.
		if(IsAPICPresent)   device::apic::sendEOI(num);
		else                device::pic8259::sendEOI(num);
	}
}
}




























