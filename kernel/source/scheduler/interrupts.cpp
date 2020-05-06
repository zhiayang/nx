// interrupts.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "devices/pc/apic.h"

namespace nx {
namespace interrupts
{
	struct irq_handler_t
	{
		int num                 = 0;
		scheduler::Thread* thr  = 0;

		irq_handler_t* next  = 0;

	};

	struct irq_state_t
	{
		irq_handler_t* handlers = 0;
	};

	struct pending_irq_t
	{
		// the irq number.
		int irq;

		// how many drivers that are pending a check-in to tell us
		// whether they ignored or handled.
		int remaining;

		bool handled;
	};

	static nx::mutex handlerLock;
	static nx::treemap<int, irq_state_t> irqHandlers;

	// we need the special allocator for this, since we call this inside IRQ handlers.
	static Synchronised<nx::list<pending_irq_t, _fixed_allocator>, nx::IRQSpinlock> inflightIRQs;

	void addIRQHandler(int irq, scheduler::Process* proc)
	{
		// TODO: for now we just... use the first thread.
		addIRQHandler(irq, &proc->threads[0]);
	}

	void addIRQHandler(int num, scheduler::Thread* thr)
	{
		autolock lk(&handlerLock);

		auto hand = new irq_handler_t();

		hand->num       = num;
		hand->thr       = thr;
		hand->next      = 0;

		if(auto it = irqHandlers.find(num); it != irqHandlers.end())
		{
			auto head = it->value.handlers;
			hand->next = head;

			it->value.handlers = hand;
		}
		else
		{
			auto st = irq_state_t();

			st.handlers = hand;
			irqHandlers.insert(num, st);
		}
	}

	void signalIRQIgnored(int num)
	{
		inflightIRQs.perform([num](auto& list) {

			// grab the first pending irq that matches, and decrease the count.
			auto it = list.find_if([num](const pending_irq_t& p) -> bool {
				return p.irq == num;
			});

			if(it != list.end())
			{
				it->remaining--;

				if(it->remaining == 0)
				{
					list.erase(it);
					if(!it->handled)
					{
						warn("irq", "no drivers handled irq %d", num);
						sendEOI(num);
					}
				}
			}
			else
			{
				error("irq", "failed to find matching inflight irq");
			}
		});
	}


	void signalIRQHandled(int num)
	{
		inflightIRQs.perform([num](auto& list) {

			sendEOI(num);

			// grab the first pending irq that matches, and decrease the count.
			auto it = list.find_if([num](const pending_irq_t& p) -> bool {
				return p.irq == num;
			});

			if(it != list.end())
			{
				it->handled = true;
				it->remaining--;

				if(it->remaining == 0)
					list.erase(it);
			}
			else
			{
				error("irq", "failed to find matching inflight irq");
			}
		});
	}


	bool processIRQ(int irq, void*)
	{
		irq_state_t* st = 0;

		if(auto it = irqHandlers.find(irq); it != irqHandlers.end())
			st = &it->value;

		else
			return false;


		bool ok = false;
		auto hand = st->handlers;

		pending_irq_t pending {
			.irq = irq,
			.remaining = 0,
			.handled = false
		};

		// just... signal all the handlers.
		while(hand)
		{
			// of course, this just queues the thread to get signalled, nothing happens immediately.
			// if the thread has no signal handler, then the driver isn't ready yet -- we need to EOI it
			// so future interrupts don't get lost.
			ok |= ipc::signalThread(hand->thr, ipc::SIGNAL_DEVICE_IRQ,
				ipc::signal_message_body_t(irq, 0, 0));

			// not sure how many times we should boost; using '5' should
			// pretty much put it at the front of the queue.
			hand->thr->priority.boost(5);
			hand = hand->next;

			pending.remaining++;
		}

		// add this.
		inflightIRQs.lock()->append(pending);

		return ok;
	}

	void init()
	{
		new (&handlerLock) nx::mutex();
		new (&irqHandlers) nx::treemap<int, irq_state_t>();

		new (&inflightIRQs) decltype(inflightIRQs)();
	}
}
}






