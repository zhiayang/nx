// lapic.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

#include "devices/acpi.h"
#include "devices/pc/apic.h"
#include "devices/pc/pit8253.h"
#include "devices/pc/pic8259.h"

// #include "math.h"

namespace nx {
namespace device {
namespace apic
{
	constexpr addr_t REG_APIC_ID        = 0x20;
	constexpr addr_t REG_EOI            = 0xB0;
	constexpr addr_t REG_SPURIOUS       = 0xF0;

	constexpr addr_t REG_ISR            = 0x100;

	constexpr addr_t REG_LVT_TIMER      = 0x320;
	constexpr addr_t REG_LVT_LINT0      = 0x350;
	constexpr addr_t REG_LVT_LINT1      = 0x360;

	constexpr addr_t REG_TIMER_INITIAL  = 0x380;
	constexpr addr_t REG_TIMER_CURRENT  = 0x390;
	constexpr addr_t REG_TIMER_DIVISOR  = 0x3E0;

	constexpr uint8_t TIMER_VECTOR = 0xFE;


	static void writeLAPIC(addr_t base, addr_t reg, uint32_t value)
	{
		*((volatile uint32_t*) (base + reg)) = value;
	}

	static uint32_t readLAPIC(addr_t base, addr_t reg)
	{
		return *((volatile uint32_t*) (base + reg));
	}

	static uint32_t readISR(addr_t base, int num)
	{
		assert(num >= IRQ_BASE_VECTOR);

		auto reg = REG_ISR + (0x10 * (num / 32));
		return readLAPIC(base, reg);
	}


	// note: this is in lapic.cpp because we ack the interrupts on the local apic.
	void sendEOI(int num)
	{
		// get the current guy
		auto proc = scheduler::getCurrentCPU();
		assert(proc);

		auto vector = num + IRQ_BASE_VECTOR;

		// check the ISR register for the interrupt:
		// note: we always take an IRQ number (ie IRQ0 is passed as '0'!)
		auto isr = readISR(proc->localApicAddr, vector);
		if(isr & (1 << (vector % 32)))
		{
			// send an eoi by writing 0 to the eoi register.
			writeLAPIC(proc->localApicAddr, REG_EOI, 0);
		}
	}

	void initLAPIC()
	{
		auto base = scheduler::getCurrentCPU()->localApicAddr;
		assert(base);

		// set the spurious vector to 0xFF, and set the LAPIC enable bit.
		writeLAPIC(base, REG_SPURIOUS, 0x100 | 0xFF);
	}

	int calibrateLAPICTimer()
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
			device::ioapic::setIRQMapping(device::ioapic::getISAIRQMapping(0), 0, 0);
		}
		else
		{
			interrupts::unmaskIRQ(0);
		}

		interrupts::enable();

		// do measurements in femtoseconds, since we don't have floating point.
		constexpr uint64_t prescale = 1'000'000;
		uint64_t femtosecondsPerTimerTick = 0;
		{
			auto measurementPeriodNS = 2 * __max(scheduler::getNanosecondsPerTick(), device::pit8253::getNanosecondsPerTick());
			auto waitingTicks = measurementPeriodNS / device::pit8253::getNanosecondsPerTick();

			// repeat the experiment 5 times, and take the average.
			constexpr int repeats = 5;

			uint64_t apic_sum = 0;
			uint64_t pit_sum = 0;

			for(int i = 0; i < repeats; i++)
			{
				auto init = readLAPIC(base, REG_TIMER_CURRENT);

				uint64_t start = device::pit8253::getTicks();
				uint64_t end = 0;
				while((end = device::pit8253::getTicks()) - start < waitingTicks)
					asm volatile ("pause");

				// read the current counter
				auto apic_elapsed = init - readLAPIC(base, REG_TIMER_CURRENT);

				// lapic ticked 'timerDiff' times in 'diff' nanoseconds
				auto pit_elapsed = (end - start) * device::pit8253::getNanosecondsPerTick();

				apic_sum += apic_elapsed;
				pit_sum += pit_elapsed;
			}

			femtosecondsPerTimerTick = (prescale * pit_sum) / apic_sum;

			// kill the old PIT
			device::pit8253::disable();
		}
		interrupts::disable();

		log("lapic", "calibrated lapic timer: ~{} fs per tick", (uint64_t) femtosecondsPerTimerTick);

		// arm the timer.
		{
			auto timerTicks = (uint32_t) (prescale * scheduler::getNanosecondsPerTick() / (femtosecondsPerTimerTick));

			log("lapic", "timer set to {} ticks", timerTicks);

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

		// timer vector is 0xFE, so it already takes the IRQ_BASE into account.
		config |= (TIMER_VECTOR) & 0xFF;    // vector
		config |= 0x20000;                  // timer mode

		writeLAPIC(base, REG_LVT_TIMER, config);

		scheduler::setTickIRQ(TIMER_VECTOR - IRQ_BASE_VECTOR);
		sendEOI(TIMER_VECTOR - IRQ_BASE_VECTOR);

		device::pic8259::sendEOI(0);
		return TIMER_VECTOR - IRQ_BASE_VECTOR;
	}
}
}
}






























