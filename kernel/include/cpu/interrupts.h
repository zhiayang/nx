// interrupts.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"

namespace nx
{
	namespace scheduler
	{
		struct Thread;
		struct Process;
	}

	namespace interrupts
	{
		void init();
		void enable();
		void disable();
		void resetNesting();

		void init_arch();

		void maskIRQ(int num);
		void unmaskIRQ(int num);

		// note: 'num' is in IRQ space! (0 => IRQ_BASE_VECTOR)
		void sendEOI(int num);

		bool hasIOAPIC();
		bool hasLocalAPIC();

		// note: 'vector' is in IRQ space! (0 => IRQ_BASE_VECTOR)
		void mapIRQVector(int irq, int vector, int apicId);

		void addIRQHandler(int irq, scheduler::Thread* thr);
		void addIRQHandler(int irq, scheduler::Process* proc);

		// returns true if there was at least one target to handle the IRQ.
		bool processIRQ(int num, void* data);

		// tells the interrupt "manager" that one of the signalled processes did not process
		// the irq successfully (eg. it was not from that particular device)
		void signalIRQIgnored(int num);

		// tells the interrupt "manager" that somebody successfully handled the device irq.
		void signalIRQHandled(int num);
	}
}



















