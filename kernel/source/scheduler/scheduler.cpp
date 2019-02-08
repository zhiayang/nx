// scheduler.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

#include "devices/pc/apic.h"
#include "devices/pc/pit8253.h"

extern "C" void nx_x64_yield_thread();
extern "C" void nx_x64_tick_handler();
extern "C" void nx_x64_switch_to_thread(uint64_t stackPtr, uint64_t cr3);

namespace nx {
namespace scheduler
{
	constexpr uint64_t NS_PER_TICK           = time::milliseconds(10).ns();
	constexpr uint64_t TIMESLICE_DURATION_NS = time::milliseconds(50).ns();

	static_assert(NS_PER_TICK <= TIMESLICE_DURATION_NS);
	static_assert(TIMESLICE_DURATION_NS % NS_PER_TICK == 0);    // not strictly necessary


	// TODO: *all* of this crap needs to be CPU-specific and probably go into the CPU structure!
	static Thread* IdleThread = 0;
	static Thread* CurrentThread = 0;

	static nx::list<Thread*> ThreadList;
	static nx::list<Thread*> BlockedThreads;
	static nx::list<Thread*> DestructionQueue;

	static nx::list<Process*> ProcessList;




	extern "C" void nx_x64_find_and_switch_thread(uint64_t stackPointer)
	{
		// save the current stack.
		auto oldcr3 = CurrentThread->parent->cr3;
		CurrentThread->kernelStack = stackPointer;

		// only put it back in the runqueue if we didn't block or sleep
		if(CurrentThread->state == ThreadState::Running)
			ThreadList.append(CurrentThread);


		if(ThreadList.empty())  CurrentThread = IdleThread;
		else                    CurrentThread = ThreadList.popFront();

		CurrentThread->state = ThreadState::Running;
		auto newcr3 = CurrentThread->parent->cr3;

		nx_x64_switch_to_thread(CurrentThread->kernelStack, oldcr3 == newcr3 ? 0 : newcr3);
	}





	[[noreturn]] static void death_destroyer_of_threads()
	{
		while(true)
		{
			while(DestructionQueue.size() > 0)
			{
				auto thr = DestructionQueue.popFront();
				assert(thr);

				destroyThread(thr);
			}

			yield();
		}
	}



	void init(Thread* idle_thread, Thread* work_thread)
	{
		IdleThread = idle_thread;

		ThreadList = nx::list<Thread*>();
		BlockedThreads = nx::list<Thread*>();
		DestructionQueue = nx::list<Thread*>();

		ProcessList = nx::list<Process*>();



		CurrentThread = work_thread;
		ThreadList.append(work_thread);

		if constexpr (getArchitecture() == Architecture::x64)
		{
			if(interrupts::hasLocalAPIC())
			{
				// setup stuff
				device::apic::initLAPIC();

				// calibrate the local apic timer for our scheduler ticks
				device::apic::calibrateLAPICTimer();
			}
			else
			{
				// re-set the PIT to fit our needs.
				device::pit8253::setPeriod(NS_PER_TICK);

				// who knows, we might have an ioapic without an lapic??
				auto irq = device::ioapic::getISAIRQMapping(0);

				interrupts::unmaskIRQ(irq);
				setTickIRQ(irq);
			}
		}
		else
		{
			abort("unsupported architecture!");
		}


		{
			// install the tick handler
			cpu::idt::setEntry(IRQ_BASE_VECTOR + 0, (addr_t) nx_x64_tick_handler,
				/* cs: */ 0x08, /* ring3: */ false, /* nestable:  */ false);

			// install the yield handler
			cpu::idt::setEntry(0xF0, (addr_t) nx_x64_yield_thread,
				/* cs: */ 0x08, /* ring3: */ false, /* nestable:  */ false);
		}

		// add our own worker thread.
		addThread(createThread(idle_thread->parent, death_destroyer_of_threads));


		nx_x64_switch_to_thread(work_thread->kernelStack, 0);
	}


	void yield()
	{
		asm volatile ("int $0xF0");
	}

	void exit()
	{
		auto t = getCurrentThread();
		assert(t);

		log("sched", "exiting thread %lu", t->threadId);

		t->state = ThreadState::AboutToExit;
		DestructionQueue.append(t);

		yield();

		while(1);
	}

	void block(mutex* mtx)
	{
		auto t = getCurrentThread();
		assert(t);

		t->blockedMtx = mtx;
		t->state = ThreadState::BlockedOnMutex;

		BlockedThreads.append(t);

		yield();
	}

	void unblock(mutex* mtx)
	{
		// hmm.
		for(auto thr : BlockedThreads)
		{
			if(thr->state == ThreadState::BlockedOnMutex && thr->blockedMtx == mtx)
			{
				thr->blockedMtx = 0;
				thr->state = ThreadState::Stopped;

				ThreadList.prepend(thr);
			}
		}
	}


	void sleep(uint64_t ns)
	{
		auto t = getCurrentThread();
		assert(t);

		t->wakeUpTimestamp = getElapsedNanoseconds() + ns;
		t->state = ThreadState::BlockedOnSleep;

		BlockedThreads.append(t);

		yield();
	}

	static void wakeUpThreads()
	{
		// checks for sleeping threads and wakes them up if necessary.
		// TODO: optimise this!!!!

		auto now = getElapsedNanoseconds();
		for(auto thr : BlockedThreads)
		{
			if(thr->state == ThreadState::BlockedOnSleep && now >= thr->wakeUpTimestamp)
			{
				thr->wakeUpTimestamp = 0;
				thr->state = ThreadState::Stopped;

				ThreadList.prepend(thr);
			}
		}
	}


	Thread* getCurrentThread()
	{
		return CurrentThread;
	}

	void addThread(Thread* t)
	{
		ThreadList.append(t);
	}











	// tick-related stuff

	static int TickIRQ = 0;
	void setTickIRQ(int irq)
	{
		TickIRQ = irq;
	}

	static uint64_t ElapsedNanoseconds = 0;
	uint64_t getElapsedNanoseconds()
	{
		return ElapsedNanoseconds;
	}

	uint64_t getNanosecondsPerTick()
	{
		return NS_PER_TICK;
	}


	// returns true if it's time to preempt somebody
	static uint64_t TickCounter = 0;
	extern "C" int nx_x64_scheduler_tick()      // note: this is called from asm-land
	{
		interrupts::sendEOI(TickIRQ);

		TickCounter += 1;
		ElapsedNanoseconds += NS_PER_TICK;

		wakeUpThreads();

		bool ret = (TickCounter * NS_PER_TICK) >= TIMESLICE_DURATION_NS;
		if(ret) TickCounter = 0;

		return ret;
	}













	static int initPhase = 0;
	int getCurrentInitialisationPhase()
	{
		return initPhase;
	}

	void initialisePhase(int p)
	{
		assert(p >= initPhase);
		initPhase = p;
	}
}
}




























