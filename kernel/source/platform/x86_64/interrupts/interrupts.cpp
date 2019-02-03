// interrupts.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "devices/x64/pit8253.h"
#include "devices/x64/pic8259.h"

extern "C" void nx_x64_irq_handler_0();
extern "C" void nx_x64_irq_handler_1();
extern "C" void nx_x64_irq_handler_2();
extern "C" void nx_x64_irq_handler_3();
extern "C" void nx_x64_irq_handler_4();
extern "C" void nx_x64_irq_handler_5();
extern "C" void nx_x64_irq_handler_6();
extern "C" void nx_x64_irq_handler_7();
extern "C" void nx_x64_irq_handler_8();
extern "C" void nx_x64_irq_handler_9();
extern "C" void nx_x64_irq_handler_10();
extern "C" void nx_x64_irq_handler_11();
extern "C" void nx_x64_irq_handler_12();
extern "C" void nx_x64_irq_handler_13();
extern "C" void nx_x64_irq_handler_14();
extern "C" void nx_x64_irq_handler_15();

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

		cpu::idt::setEntry(IRQ_BASE_VECTOR + 0,     (addr_t) nx_x64_irq_handler_0,  0x08, 0x8E);
		cpu::idt::setEntry(IRQ_BASE_VECTOR + 1,     (addr_t) nx_x64_irq_handler_1,  0x08, 0x8E);
		cpu::idt::setEntry(IRQ_BASE_VECTOR + 2,     (addr_t) nx_x64_irq_handler_2,  0x08, 0x8E);
		cpu::idt::setEntry(IRQ_BASE_VECTOR + 3,     (addr_t) nx_x64_irq_handler_3,  0x08, 0x8E);
		cpu::idt::setEntry(IRQ_BASE_VECTOR + 4,     (addr_t) nx_x64_irq_handler_4,  0x08, 0x8E);
		cpu::idt::setEntry(IRQ_BASE_VECTOR + 5,     (addr_t) nx_x64_irq_handler_5,  0x08, 0x8E);
		cpu::idt::setEntry(IRQ_BASE_VECTOR + 6,     (addr_t) nx_x64_irq_handler_6,  0x08, 0x8E);
		cpu::idt::setEntry(IRQ_BASE_VECTOR + 7,     (addr_t) nx_x64_irq_handler_7,  0x08, 0x8E);
		cpu::idt::setEntry(IRQ_BASE_VECTOR + 8,     (addr_t) nx_x64_irq_handler_8,  0x08, 0x8E);
		cpu::idt::setEntry(IRQ_BASE_VECTOR + 9,     (addr_t) nx_x64_irq_handler_9,  0x08, 0x8E);
		cpu::idt::setEntry(IRQ_BASE_VECTOR + 10,    (addr_t) nx_x64_irq_handler_10, 0x08, 0x8E);
		cpu::idt::setEntry(IRQ_BASE_VECTOR + 11,    (addr_t) nx_x64_irq_handler_11, 0x08, 0x8E);
		cpu::idt::setEntry(IRQ_BASE_VECTOR + 12,    (addr_t) nx_x64_irq_handler_12, 0x08, 0x8E);
		cpu::idt::setEntry(IRQ_BASE_VECTOR + 13,    (addr_t) nx_x64_irq_handler_13, 0x08, 0x8E);
		cpu::idt::setEntry(IRQ_BASE_VECTOR + 14,    (addr_t) nx_x64_irq_handler_14, 0x08, 0x8E);
		cpu::idt::setEntry(IRQ_BASE_VECTOR + 15,    (addr_t) nx_x64_irq_handler_15, 0x08, 0x8E);
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

	void sendEOI(int num)
	{
		if(IsAPICPresent)   device::apic::sendEOI(num);
		else                device::pic8259::sendEOI(num);
	}

	extern "C" void nx_x64_send_eoi(int num)
	{
		sendEOI(num);
	}

	extern "C" void nx_x64_handle_irq(int num)
	{
		if(num == 0) device::pit8253::tick();

		nx_x64_send_eoi(num);
	}
}
}




























