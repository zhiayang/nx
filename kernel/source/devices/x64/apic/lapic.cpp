// lapic.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

#include "devices/acpi.h"
#include "devices/x64/apic.h"

namespace nx {
namespace device {
namespace apic
{
	static constexpr int LAPIC_REG_EOI      = 0xB0;
	static constexpr int LAPIC_REG_SPURIOUS = 0xF0;

	static constexpr int LAPIC_REG_TIMER    = 0x320;
	static constexpr int LAPIC_REG_LINT0    = 0x350;
	static constexpr int LAPIC_REG_LINT1    = 0x360;


	static void writeLAPIC(addr_t base, int reg, uint32_t value)
	{
		*((volatile uint32_t*) (base + (addr_t) reg)) = value;
	}

	static uint32_t readLAPIC(addr_t base, int reg)
	{
		return *((volatile uint32_t*) (base + (addr_t) reg));
	}




	// note: this is in lapic.cpp because we ack the interrupts on the local apic.
	void sendEOI(int num)
	{
		// get the current guy
		auto proc = scheduler::getCurrentProcessor();
		assert(proc);

		// send an eoi by writing 0 to the eoi register.
		writeLAPIC(proc->localApicAddr, LAPIC_REG_EOI, 0);
	}
}
}
}