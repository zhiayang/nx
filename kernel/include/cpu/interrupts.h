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
	}

	namespace interrupts
	{
		void init();
		void enable();
		void disable();

		void init_arch();

		void maskIRQ(int num);
		void unmaskIRQ(int num);

		// note: 'num' is in IRQ space! (0 => IRQ_BASE_VECTOR)
		void sendEOI(int num);

		bool hasIOAPIC();
		bool hasLocalAPIC();

		// note: 'vector' is in IRQ space! (0 => IRQ_BASE_VECTOR)
		void mapIRQVector(int irq, int vector, int apicId);

		// note: the handler can return false to stop further processing.
		void addIRQHandler(int irq, int priority, bool (*)(int, void*));

		// this one discards the new IRQ if we are full. returns false if
		// we had to discard anything.
		bool enqueueIRQ(int num, void* data);

		// this one discards the oldest pending IRQ if we are full.
		// returns false if we had to discard anything.
		bool enqueueImptIRQ(int num, void* data);

		bool hasPendingIRQs();
		void processIRQ();

		scheduler::Thread* getHandlerThread();
	}
}



















