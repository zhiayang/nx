// lapic.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

#include "devices/acpi.h"
#include "devices/x64/apic.h"
#include "devices/x64/pit8253.h"

#include "math.h"

namespace nx {
namespace device {
namespace apic
{
	constexpr int REG_EOI            = 0xB0;
	constexpr int REG_SPURIOUS       = 0xF0;

	constexpr int REG_LVT_TIMER      = 0x320;
	constexpr int REG_LVT_LINT0      = 0x350;
	constexpr int REG_LVT_LINT1      = 0x360;

	constexpr int REG_TIMER_INITIAL  = 0x380;
	constexpr int REG_TIMER_CURRENT  = 0x390;
	constexpr int REG_TIMER_DIVISOR  = 0x3E0;


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

	void initLAPIC()
	{
		auto base = scheduler::getCurrentCPU()->localApicAddr;
		assert(base);

		// set the spurious vector to 0xFF, and set the LAPIC enable bit.
		writeLAPIC(base, REG_SPURIOUS, 0x100 | 0xFF);
	}

	void calibrateLAPICTimer()
	{
		auto base = scheduler::getCurrentCPU()->localApicAddr;
		assert(base);

		// 001b divides by 4 -- so we'll just use that. the rest are reserved.
		writeLAPIC(base, REG_TIMER_DIVISOR, 0x1);

		// set a big initial value
		writeLAPIC(base, REG_TIMER_INITIAL, 0xFFFFFFFF);
		writeLAPIC(base, REG_TIMER_CURRENT, 0xFFFFFFFF);


		// setup the PIT
		device::pit8253::enable();

		if(interrupts::hasIOAPIC())
		{
			device::ioapic::setInterrupt(device::ioapic::getISAIRQMapping(0),
				IRQ_BASE_VECTOR, 0);
		}
		else
		{
			interrupts::unmaskIRQ(0);
		}

		double nanosecondsPerTimerTick = 0;
		{
			auto measurementPeriodNS = scheduler::getNanosecondsPerTick();
			auto waitingTicks = measurementPeriodNS / device::pit8253::getNanosecondsPerTick();

			constexpr int repeats = 5;

			// repeat the experiment 5 times
			double resultSum = 0;
			for(int i = 0; i < repeats; i++)
			{
				auto init = readLAPIC(base, REG_TIMER_CURRENT);

				uint64_t start = device::pit8253::getTicks();
				uint64_t end = 0;
				while((end = device::pit8253::getTicks()) - start < waitingTicks)
					asm volatile ("pause");

				// read the current counter
				auto diff = init - readLAPIC(base, REG_TIMER_CURRENT);

				// lapic ticked 'timerDiff' times in 'diff' nanoseconds
				auto elapsed = (end - start) * device::pit8253::getNanosecondsPerTick();

				resultSum += (double) elapsed / (double) diff;
			}

			nanosecondsPerTimerTick = resultSum / (double) repeats;

			// kill the old PIT
			device::pit8253::disable();
		}



		log("lapic", "calibrated lapic timer: %.2f ns per tick", nanosecondsPerTimerTick);


		// arm the timer.
		{
			auto timerTicks = (uint32_t) (scheduler::getNanosecondsPerTick() / nanosecondsPerTimerTick);

			log("lapic", "timer set to %u ticks", timerTicks);

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






























