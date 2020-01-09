// interrupts.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace interrupts
{
	struct irq_handler_t
	{
		int num                     = 0;
		int priority                = 0;
		bool (*handler)(int, void*) = 0;

		irq_handler_t* next  = 0;
	};

	struct irq_event_t
	{
		int irq            = 0;
		void* data         = 0;
		uint64_t timestamp = 0;
	};

	static constexpr size_t IRQ_BUFFER_SIZE = 1024;

	struct irq_state_t
	{
		irq_handler_t* handlers = 0;
	};

	struct irq_event_buffer_t
	{
		size_t head = 0;
		size_t tail = 0;
		bool full = false;

		irq_event_t buffer[IRQ_BUFFER_SIZE];
	};

	static size_t pendingIRQs = 0;
	static irq_event_buffer_t irqBuffer;

	static nx::treemap<int, irq_state_t> irqHandlers;

	static irq_handler_t* insertHandler(irq_handler_t* begin, irq_handler_t* x)
	{
		auto h = begin;

		while(h != 0)
		{
			if(h->priority <= x->priority && (!h->next || h->next->priority > x->priority))
			{
				x->next = h->next;
				h->next = x;

				return begin;
			}
			else if(x->priority < h->priority)
			{
				x->next = h;
				return x;
			}

			h = h->next;
		}

		return begin;
	}

	void addIRQHandler(int num, int priority, bool (*callback)(int, void*))
	{
		auto hand = new irq_handler_t();

		hand->num      = num;
		hand->priority = priority;
		hand->handler  = callback;
		hand->next     = 0;

		if(auto it = irqHandlers.find(num); it != irqHandlers.end())
		{
			it->value.handlers = insertHandler(it->value.handlers, hand);
		}
		else
		{
			auto st = irq_state_t();
			st.handlers = hand;

			irqHandlers.insert(num, st);
		}
	}

	static void advance_ptr()
	{
		if(irqBuffer.full)
		{
			irqBuffer.tail = (irqBuffer.tail + 1) % IRQ_BUFFER_SIZE;
		}

		irqBuffer.head = (irqBuffer.head + 1) % IRQ_BUFFER_SIZE;
		irqBuffer.full = (irqBuffer.head == irqBuffer.tail);
	}

	static void retreat_ptr()
	{
		irqBuffer.full = false;
		irqBuffer.tail = (irqBuffer.tail + 1) % IRQ_BUFFER_SIZE;
	}

	// TODO: NEEDS LOCKING


	// this discards the incoming (new) IRQ if the buffer was full.
	bool enqueueIRQ(int irq, void* data)
	{
		if(irqBuffer.full)
			return false;

		pendingIRQs += 1;
		irqBuffer.buffer[irqBuffer.head] = {
			.irq       = irq,
			.data      = data,
			.timestamp = scheduler::getElapsedNanoseconds()
		};

		advance_ptr();
		serial::debugprintf("one irq (%d)", pendingIRQs);

		return true;
	}

	// this discards old pending IRQs if the buffer was full.
	// returns false if we had to discard.
	bool enqueueImptIRQ(int irq, void* data)
	{
		bool ret = !(irqBuffer.full);

		// note: if we had to discard, then the number of pending irqs
		// doesn't change.
		if(ret) pendingIRQs += 1;

		irqBuffer.buffer[irqBuffer.head] = {
			.irq       = irq,
			.data      = data,
			.timestamp = scheduler::getElapsedNanoseconds()
		};

		advance_ptr();
		return ret;
	}

	bool hasPendingIRQs()
	{
		return pendingIRQs > 0;
	}

	size_t getPendingIRQs()
	{
		return pendingIRQs;
	}

	void processIRQ()
	{
		if(pendingIRQs == 0)
			return;

		assert(irqBuffer.full || (irqBuffer.head != irqBuffer.tail));
		pendingIRQs--;

		auto event = irqBuffer.buffer[irqBuffer.tail];
		retreat_ptr();

		serial::debugprintf("irq: %d / %d", event.irq, pendingIRQs);
		port::read1b(0x60);

		irq_state_t* st = 0;

		if(auto it = irqHandlers.find(event.irq); it != irqHandlers.end())
			st = &it->value;

		else
			return;

		auto hand = st->handlers;

		while(hand)
		{
			bool cont = hand->handler(event.irq, event.data);
			if(!cont) break;

			hand = hand->next;
		}
	}


	static scheduler::Thread* workerThread = 0;

	scheduler::Thread* getHandlerThread()
	{
		assert(workerThread);
		return workerThread;
	}

	static void irq_sched()
	{
		while(true)
		{
			if(hasPendingIRQs())    processIRQ();
			else                    scheduler::yield();
		}
	}

	void init()
	{
		irqHandlers = nx::treemap<int, irq_state_t>();
		memset(&irqBuffer, 0, sizeof(irq_event_buffer_t));

		workerThread = scheduler::createThread(scheduler::getKernelProcess(), irq_sched);
		// scheduler::addThread(workerThread);
	}
}
}






