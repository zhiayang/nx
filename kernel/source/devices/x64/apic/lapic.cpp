// lapic.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

#include "devices/acpi.h"
#include "devices/x64/apic.h"
#include "devices/x64/pit8253.h"

#include "misc/timekeeping.h"

#include "math.h"

namespace nx {
namespace device {
namespace apic
{
	static constexpr int REG_EOI            = 0xB0;
	static constexpr int REG_SPURIOUS       = 0xF0;

	static constexpr int REG_LVT_TIMER      = 0x320;
	static constexpr int REG_LVT_LINT0      = 0x350;
	static constexpr int REG_LVT_LINT1      = 0x360;

	static constexpr int REG_TIMER_INITIAL  = 0x380;
	static constexpr int REG_TIMER_CURRENT  = 0x390;
	static constexpr int REG_TIMER_DIVISOR  = 0x3E0;


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
		auto proc = scheduler::getCurrentCPU();
		assert(proc);

		// send an eoi by writing 0 to the eoi register.
		writeLAPIC(proc->localApicAddr, REG_EOI, 0);
	}

	void initLAPICTimer()
	{
		auto base = scheduler::getCurrentCPU()->localApicAddr;
		assert(base);

		// 001b divides by 4 -- so we'll just use that. the rest are reserved.
		writeLAPIC(base, REG_TIMER_DIVISOR, 0x1);

		// set a big initial value
		writeLAPIC(base, REG_TIMER_INITIAL, 0xFFFFFFFF);
		writeLAPIC(base, REG_TIMER_CURRENT, 0xFFFFFFFF);


		// setup the PIT
		{
			int irq = device::apic::getISAIRQMapping(0);

			device::pit8253::enable();
			device::apic::setInterrupt(irq, IRQ_BASE_VECTOR, 0);
		}


		constexpr uint64_t measurementPeriodNS = time::milliseconds(50).ns();
		auto waitingTicks = measurementPeriodNS / device::pit8253::getNanosecondsPerTick();

		// println("pit: %zu ns per tick (waiting %zu ticks)", device::pit8253::getNanosecondsPerTick(), waitingTicks);


		// repeat the experiment 5 times
		double results[5] = { };
		uint32_t timerDiffs[5] = { };

		for(int i = 0; i < 5; i++)
		{
			// set the initial count to something big
			auto init = readLAPIC(base, REG_TIMER_CURRENT);

			uint64_t start = device::pit8253::getTicks();
			uint64_t end = 0;
			while((end = device::pit8253::getTicks()) - start < waitingTicks)
				asm volatile ("pause");

			// read the LAPIC current.
			auto curr = readLAPIC(base, REG_TIMER_CURRENT);
			timerDiffs[i] = init - curr;

			// lapic ticked 'timerDiff' times in 'diff' nanoseconds
			auto elapsed = (end - start) * device::pit8253::getNanosecondsPerTick();

			results[i] = (double) elapsed / (double) timerDiffs[i];
		}

		auto nsPerTick = (results[0] + results[1] + results[2] + results[3] + results[4]) / 5.0;
		log("lapic", "calibrated lapic timer: %.2f ns per tick", nsPerTick);

		// kill the old PIT
		device::pit8253::disable();

		// arm the timer.
		{
			auto timerTicks = (timerDiffs[0] + timerDiffs[1] + timerDiffs[2] + timerDiffs[3] + timerDiffs[4]) / 5;
			timerTicks *= (scheduler::getNanosecondsPerTick() / measurementPeriodNS);

			assert(timerTicks <= 0xFFFFFFFF);

			writeLAPIC(base, REG_TIMER_DIVISOR, 0x1);
			writeLAPIC(base, REG_TIMER_INITIAL, timerTicks);
		}


		// set the interrupt to trigger on IRQ0, periodic mode, and unmask it.
		uint32_t config = 0;

		// 0:7      -- vector
		// 8:11     -- delivery mode (000 for normal ie fixed)
		// ...
		// 16       -- set to mask
		// 17:18    -- timer mode (00 = one-shot, 01 = periodic, 10 = tsc deadline)

		config |= (IRQ_BASE_VECTOR + 0) & 0xFF;     // vector
		config |= 0x20000;                          // timer mode


		writeLAPIC(base, REG_LVT_TIMER, config);
		scheduler::setTickIRQ(0);
		sendEOI(0);
	}
}
}
}






























