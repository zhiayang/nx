// scheduler.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace scheduler
{
	static int TickIRQ = 0;
	void setTickIRQ(int irq)
	{
		TickIRQ = irq;
	}


	static uint64_t ticks = 0;

	extern "C" void nx_x64_yield_thread();
	extern "C" void nx_x64_tick_handler();
	extern "C" void nx_x64_switch_to_thread(uint64_t stackPtr, uint64_t cr3);

	extern "C" int nx_x64_scheduler_tick()
	{
		interrupts::sendEOI(TickIRQ);

		return ++ticks % 200 == 0;
	}

	static Thread* IdleThread = 0;
	static Thread* CurrentThread = 0;
	static nx::list<Thread*> ThreadList;

	void init(Thread* idle_thread, Thread* work_thread)
	{
		IdleThread = idle_thread;

		ThreadList = nx::list<Thread*>();
		ThreadList.append(work_thread);

		// install the tick handler
		cpu::idt::setEntry(0x20, (addr_t) nx_x64_tick_handler, 0x08, 0x8E);

		// install the yield handler
		cpu::idt::setEntry(0xF0, (addr_t) nx_x64_yield_thread, 0x08, 0x8E);

		// switch to the idle thread.
		CurrentThread = work_thread;
		nx_x64_switch_to_thread(work_thread->kernelStack, 0);
	}




	extern "C" void nx_x64_find_and_switch_thread(uint64_t stackPointer)
	{
		// save the current stack.
		getCurrentThread()->kernelStack = stackPointer;

		auto next = getNextThread();
		CurrentThread = next;

		// println("(switch %ld) (%p)", next->threadId, next->kernelStack);
		nx_x64_switch_to_thread(next->kernelStack, 0);
	}


	void yield()
	{
		asm volatile ("int $0xF0");
	}

	void block(mutex* mtx)
	{
		auto t = getCurrentThread();
		assert(t);

		t->flags |= FLAG_MUTEX_BLOCK;
		t->blockedMtx = mtx;

		yield();
	}

	void unblock(mutex* mtx)
	{
		// hmm.
	}


	Thread* getNextThread()
	{
		auto ret = ThreadList.popFront();
		ThreadList.append(ret);

		return ret;
	}


	Thread* getCurrentThread()
	{
		return CurrentThread;
	}











	void add(Thread* t)
	{
		ThreadList.append(t);
	}

	void add(Process* p)
	{
		for(auto& t : p->threads)
			add(&t);
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




























