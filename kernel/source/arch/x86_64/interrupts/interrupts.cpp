// interrupts.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

#include "devices/pc/pit8253.h"
#include "devices/pc/pic8259.h"
#include "devices/pc/apic.h"

#include "handler_decls.h"

// this is annoying.
constexpr void (*irq_handlers[256 - nx::IRQ_BASE_VECTOR])() = {
	nx_x64_irq_handler_0x20, nx_x64_irq_handler_0x21, nx_x64_irq_handler_0x22, nx_x64_irq_handler_0x23,
	nx_x64_irq_handler_0x24, nx_x64_irq_handler_0x25, nx_x64_irq_handler_0x26, nx_x64_irq_handler_0x27,
	nx_x64_irq_handler_0x28, nx_x64_irq_handler_0x29, nx_x64_irq_handler_0x2A, nx_x64_irq_handler_0x2B,
	nx_x64_irq_handler_0x2C, nx_x64_irq_handler_0x2D, nx_x64_irq_handler_0x2E, nx_x64_irq_handler_0x2F,
	nx_x64_irq_handler_0x30, nx_x64_irq_handler_0x31, nx_x64_irq_handler_0x32, nx_x64_irq_handler_0x33,
	nx_x64_irq_handler_0x34, nx_x64_irq_handler_0x35, nx_x64_irq_handler_0x36, nx_x64_irq_handler_0x37,
	nx_x64_irq_handler_0x38, nx_x64_irq_handler_0x39, nx_x64_irq_handler_0x3A, nx_x64_irq_handler_0x3B,
	nx_x64_irq_handler_0x3C, nx_x64_irq_handler_0x3D, nx_x64_irq_handler_0x3E, nx_x64_irq_handler_0x3F,
	nx_x64_irq_handler_0x40, nx_x64_irq_handler_0x41, nx_x64_irq_handler_0x42, nx_x64_irq_handler_0x43,
	nx_x64_irq_handler_0x44, nx_x64_irq_handler_0x45, nx_x64_irq_handler_0x46, nx_x64_irq_handler_0x47,
	nx_x64_irq_handler_0x48, nx_x64_irq_handler_0x49, nx_x64_irq_handler_0x4A, nx_x64_irq_handler_0x4B,
	nx_x64_irq_handler_0x4C, nx_x64_irq_handler_0x4D, nx_x64_irq_handler_0x4E, nx_x64_irq_handler_0x4F,
	nx_x64_irq_handler_0x50, nx_x64_irq_handler_0x51, nx_x64_irq_handler_0x52, nx_x64_irq_handler_0x53,
	nx_x64_irq_handler_0x54, nx_x64_irq_handler_0x55, nx_x64_irq_handler_0x56, nx_x64_irq_handler_0x57,
	nx_x64_irq_handler_0x58, nx_x64_irq_handler_0x59, nx_x64_irq_handler_0x5A, nx_x64_irq_handler_0x5B,
	nx_x64_irq_handler_0x5C, nx_x64_irq_handler_0x5D, nx_x64_irq_handler_0x5E, nx_x64_irq_handler_0x5F,
	nx_x64_irq_handler_0x60, nx_x64_irq_handler_0x61, nx_x64_irq_handler_0x62, nx_x64_irq_handler_0x63,
	nx_x64_irq_handler_0x64, nx_x64_irq_handler_0x65, nx_x64_irq_handler_0x66, nx_x64_irq_handler_0x67,
	nx_x64_irq_handler_0x68, nx_x64_irq_handler_0x69, nx_x64_irq_handler_0x6A, nx_x64_irq_handler_0x6B,
	nx_x64_irq_handler_0x6C, nx_x64_irq_handler_0x6D, nx_x64_irq_handler_0x6E, nx_x64_irq_handler_0x6F,
	nx_x64_irq_handler_0x70, nx_x64_irq_handler_0x71, nx_x64_irq_handler_0x72, nx_x64_irq_handler_0x73,
	nx_x64_irq_handler_0x74, nx_x64_irq_handler_0x75, nx_x64_irq_handler_0x76, nx_x64_irq_handler_0x77,
	nx_x64_irq_handler_0x78, nx_x64_irq_handler_0x79, nx_x64_irq_handler_0x7A, nx_x64_irq_handler_0x7B,
	nx_x64_irq_handler_0x7C, nx_x64_irq_handler_0x7D, nx_x64_irq_handler_0x7E, nx_x64_irq_handler_0x7F,
	nx_x64_irq_handler_0x80, nx_x64_irq_handler_0x81, nx_x64_irq_handler_0x82, nx_x64_irq_handler_0x83,
	nx_x64_irq_handler_0x84, nx_x64_irq_handler_0x85, nx_x64_irq_handler_0x86, nx_x64_irq_handler_0x87,
	nx_x64_irq_handler_0x88, nx_x64_irq_handler_0x89, nx_x64_irq_handler_0x8A, nx_x64_irq_handler_0x8B,
	nx_x64_irq_handler_0x8C, nx_x64_irq_handler_0x8D, nx_x64_irq_handler_0x8E, nx_x64_irq_handler_0x8F,
	nx_x64_irq_handler_0x90, nx_x64_irq_handler_0x91, nx_x64_irq_handler_0x92, nx_x64_irq_handler_0x93,
	nx_x64_irq_handler_0x94, nx_x64_irq_handler_0x95, nx_x64_irq_handler_0x96, nx_x64_irq_handler_0x97,
	nx_x64_irq_handler_0x98, nx_x64_irq_handler_0x99, nx_x64_irq_handler_0x9A, nx_x64_irq_handler_0x9B,
	nx_x64_irq_handler_0x9C, nx_x64_irq_handler_0x9D, nx_x64_irq_handler_0x9E, nx_x64_irq_handler_0x9F,
	nx_x64_irq_handler_0xA0, nx_x64_irq_handler_0xA1, nx_x64_irq_handler_0xA2, nx_x64_irq_handler_0xA3,
	nx_x64_irq_handler_0xA4, nx_x64_irq_handler_0xA5, nx_x64_irq_handler_0xA6, nx_x64_irq_handler_0xA7,
	nx_x64_irq_handler_0xA8, nx_x64_irq_handler_0xA9, nx_x64_irq_handler_0xAA, nx_x64_irq_handler_0xAB,
	nx_x64_irq_handler_0xAC, nx_x64_irq_handler_0xAD, nx_x64_irq_handler_0xAE, nx_x64_irq_handler_0xAF,
	nx_x64_irq_handler_0xB0, nx_x64_irq_handler_0xB1, nx_x64_irq_handler_0xB2, nx_x64_irq_handler_0xB3,
	nx_x64_irq_handler_0xB4, nx_x64_irq_handler_0xB5, nx_x64_irq_handler_0xB6, nx_x64_irq_handler_0xB7,
	nx_x64_irq_handler_0xB8, nx_x64_irq_handler_0xB9, nx_x64_irq_handler_0xBA, nx_x64_irq_handler_0xBB,
	nx_x64_irq_handler_0xBC, nx_x64_irq_handler_0xBD, nx_x64_irq_handler_0xBE, nx_x64_irq_handler_0xBF,
	nx_x64_irq_handler_0xC0, nx_x64_irq_handler_0xC1, nx_x64_irq_handler_0xC2, nx_x64_irq_handler_0xC3,
	nx_x64_irq_handler_0xC4, nx_x64_irq_handler_0xC5, nx_x64_irq_handler_0xC6, nx_x64_irq_handler_0xC7,
	nx_x64_irq_handler_0xC8, nx_x64_irq_handler_0xC9, nx_x64_irq_handler_0xCA, nx_x64_irq_handler_0xCB,
	nx_x64_irq_handler_0xCC, nx_x64_irq_handler_0xCD, nx_x64_irq_handler_0xCE, nx_x64_irq_handler_0xCF,
	nx_x64_irq_handler_0xD0, nx_x64_irq_handler_0xD1, nx_x64_irq_handler_0xD2, nx_x64_irq_handler_0xD3,
	nx_x64_irq_handler_0xD4, nx_x64_irq_handler_0xD5, nx_x64_irq_handler_0xD6, nx_x64_irq_handler_0xD7,
	nx_x64_irq_handler_0xD8, nx_x64_irq_handler_0xD9, nx_x64_irq_handler_0xDA, nx_x64_irq_handler_0xDB,
	nx_x64_irq_handler_0xDC, nx_x64_irq_handler_0xDD, nx_x64_irq_handler_0xDE, nx_x64_irq_handler_0xDF,
	nx_x64_irq_handler_0xE0, nx_x64_irq_handler_0xE1, nx_x64_irq_handler_0xE2, nx_x64_irq_handler_0xE3,
	nx_x64_irq_handler_0xE4, nx_x64_irq_handler_0xE5, nx_x64_irq_handler_0xE6, nx_x64_irq_handler_0xE7,
	nx_x64_irq_handler_0xE8, nx_x64_irq_handler_0xE9, nx_x64_irq_handler_0xEA, nx_x64_irq_handler_0xEB,
	nx_x64_irq_handler_0xEC, nx_x64_irq_handler_0xED, nx_x64_irq_handler_0xEE, nx_x64_irq_handler_0xEF,
	nx_x64_irq_handler_0xF0, nx_x64_irq_handler_0xF1, nx_x64_irq_handler_0xF2, nx_x64_irq_handler_0xF3,
	nx_x64_irq_handler_0xF4, nx_x64_irq_handler_0xF5, nx_x64_irq_handler_0xF6, nx_x64_irq_handler_0xF7,
	nx_x64_irq_handler_0xF8, nx_x64_irq_handler_0xF9, nx_x64_irq_handler_0xFA, nx_x64_irq_handler_0xFB,
	nx_x64_irq_handler_0xFC, nx_x64_irq_handler_0xFD, nx_x64_irq_handler_0xFE, nx_x64_irq_handler_0xFF,
};



namespace nx {
namespace interrupts
{
	static bool IsIOAPICPresent = false;
	static bool IsLocalAPICPresent = false;

	bool hasIOAPIC()
	{
		return IsIOAPICPresent;
	}

	bool hasLocalAPIC()
	{
		return IsLocalAPICPresent;
	}

	void mapIRQVector(int irq, int vector, int apicId)
	{
		if(hasIOAPIC()) device::ioapic::setIRQMapping(irq, vector, apicId);
		else            error("intr", "IRQs cannot be re-mapped without an IOAPIC");
	}

	void init_arch()
	{
		if(IsIOAPICPresent = device::ioapic::init(); IsIOAPICPresent)
		{
			// disable the legacy PIC by masking all interrupts.
			device::pic8259::disable();
		}
		else
		{
			warn("apic", "system does not have an ioapic; falling back to 8259 PIC");
			device::pic8259::init();
		}

		// check for local apic
		IsLocalAPICPresent = (cpu::hasFeature(cpu::Feature::LocalAPIC) && scheduler::getCurrentCPU()->localApicAddr != 0);
		if(!IsLocalAPICPresent)
			warn("apic", "cpu does not have a local apic; this is not a well-supported configuration!");


		// map all the 224 IRQs in the IDT to the default handlers. they should just do nothing.
		for(int num = IRQ_BASE_VECTOR; num < 256; num++)
		{
			cpu::idt::setEntry(num, (addr_t) irq_handlers[num - IRQ_BASE_VECTOR],
				/* cs: */ 0x08, /* ring3: */ false, /* nestable: */ false);
		}
	}


	static int32_t early_sti_level = 0;
	static int32_t& get_sti_level()
	{
		if(__likely(scheduler::getInitPhase() >= scheduler::SchedulerInitPhase::SchedulerStarted))
			return scheduler::getCPULocalState()->stiLevel;

		return early_sti_level;
	}


	void enable()
	{
		auto& stilvl = get_sti_level();
		stilvl += 1;

		if(stilvl >= 0 && !platform::is_interrupted())
			asm volatile("sti");
	}

	void disable()
	{
		auto& stilvl = get_sti_level();

		asm volatile("cli");
		stilvl -= 1;
	}

	void resetNesting()
	{
		get_sti_level() = 0;
	}


	//! ACHTUNG !
	//* note! we expect the interrupt controller to handle the irq -> idt translation by itself!
	//* meaning to say 'num' here will be 0-based -- it is the IRQ number after all, not the interrupt number.
	void maskIRQ(int num)
	{
		if(IsIOAPICPresent) device::ioapic::maskIRQ(num);
		else                device::pic8259::maskIRQ(num);
	}

	void unmaskIRQ(int num)
	{
		if(IsIOAPICPresent) device::ioapic::unmaskIRQ(num);
		else                device::pic8259::unmaskIRQ(num);
	}

	// 'num' is a relative vector!
	void sendEOI(int num)
	{
		// note: eois are sent to the local apic!
		// to handle cases where we have lapics but no ioapics, we will always send an EOI to the lapic
		// if there's no ioapic, we will send an EOI to the 8259 pic also.

		if(IsLocalAPICPresent)
			device::apic::sendEOI(num);

		if(!IsIOAPICPresent)
			device::pic8259::sendEOI(num);
	}


	extern "C" void nx_x64_handle_irq(int num)
	{
		if(num == scheduler::getTickIRQ())
		{
			// this only happens during the early-phase, when we need to
			// calibrate the LAPIC using the PIT. it's ok to just leave
			// it here i guess.
			device::pit8253::tick();
			sendEOI(num);
		}
		else
		{
			// if there was a handler for the given IRQ,
			// processIRQ will return true -- so we don't need to EOI ourselves.
			if(!processIRQ(num, nullptr))
				sendEOI(num);
		}
	}
}
}




























