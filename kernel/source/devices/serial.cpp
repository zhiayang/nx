// serial.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

#include "devices/ports.h"
#include "devices/serial.h"
#include "devices/pc/apic.h"
#include "devices/pc/pic8259.h"

#include <stdarg.h>

extern "C" void nx_x64_serial_interrupt();

namespace nx {
namespace serial
{
	constexpr struct {
		uint16_t port   = 0x3F8;
		uint8_t irq     = 4;
	} COM1;

	constexpr uint16_t QEMU_DEBUG_SERIAL_PORT    = 0xE9;

	void init()
	{
		// disable interrupts
		port::write1b(COM1.port + 1, 0x00);

		// enable DLAB to set divisor
		port::write1b(COM1.port + 3, 0x80);

		// set divisor to 1 == baud rate 115200
		port::write1b(COM1.port + 0, 0x01);  // low byte
		port::write1b(COM1.port + 1, 0x00);  // high byte

		// mode: 8 bits, no parity, 1 stop. also, disable DLAB
		port::write1b(COM1.port + 3, 0x03);

		// enable and clear FIFOs, set 4-byte interrupt threshold
		port::write1b(COM1.port + 2, 0x47);

		// enable IRQ (aux output 2), set RTS and DTR
		port::write1b(COM1.port + 4, 0x0B);

		// ok. interrupts are still disabled, because we don't expect to receive anything just yet.
	}

	static void (*interrupt_handler)(uint8_t) = 0;
	void irq_handler()
	{
		auto c = port::read1b(COM1.port);
		if(interrupt_handler) interrupt_handler(c);

		interrupts::sendEOI(4);
	}

	void initReceive(void (*callback)(uint8_t))
	{
		interrupt_handler = callback;

		// set the interrupt to fire when data is available
		port::write1b(COM1.port + 1, 0x01);

		// setup the interrupt handler itself.
		// we bypass all the IRQ handling mechanisms, because this is the debug console.

		constexpr int IRQ_NUM = 4;

		// note: this is safe to call even if we don't have an ioapic; in which case it'll return the raw ISA irq number
		auto irq = device::ioapic::getISAIRQMapping(COM1.irq);
		device::ioapic::setIRQMapping(irq, IRQ_NUM, 0);

		cpu::idt::setEntry(IRQ_BASE_VECTOR + IRQ_NUM, (addr_t) nx_x64_serial_interrupt,
			/* cs: */ 0x08, /* ring3: */ false, /* nestable: */ false, /* ist: */ 2);

		interrupts::unmaskIRQ(irq);
	}













	void debugprint(char c)
	{
		port::write1b(QEMU_DEBUG_SERIAL_PORT, c);
		while(!(port::read1b(COM1.port + 5) & 0x20))
			;

		port::write1b(COM1.port, c);
	}

	void debugprint(char* s, size_t l)
	{
		if(!s) return;
		for(size_t i = 0; i < l && s[i]; i++)
			debugprint(s[i]);
	}

	void debugprint(const char* s)
	{
		debugprint((char*) s, strlen(s));
	}


	void debugprintf(const char* fmt, ...)
	{
		va_list args; va_start(args, fmt);

		vcbprintf(nullptr, [](void* ctx, const char* s, size_t len) -> size_t {
			serial::debugprint((char*) s, len);
			return len;
		}, fmt, args);

		va_end(args);
	}
}
}













