// scheduler.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"


extern "C" void nx_x64_yield_thread();
extern "C" void nx_x64_tick_handler();
extern "C" void nx_x64_switch_to_thread(uint64_t stackPtr, uint64_t cr3);

namespace nx {
namespace scheduler
{
	static Thread* IdleThread = 0;
	static Thread* CurrentThread = 0;

	static nx::list<Thread*> ThreadList;
	static nx::list<Thread*> BlockedThreads;


	void init(Thread* idle_thread, Thread* work_thread)
	{
		IdleThread = idle_thread;

		ThreadList = nx::list<Thread*>();
		BlockedThreads = nx::list<Thread*>();

		CurrentThread = work_thread;
		ThreadList.append(work_thread);

		// install the tick handler
		cpu::idt::setEntry(IRQ_BASE_VECTOR + 0, (addr_t) nx_x64_tick_handler, 0x08, 0x8E);

		// install the yield handler
		cpu::idt::setEntry(0xF0, (addr_t) nx_x64_yield_thread, 0x08, 0x8E);


		nx_x64_switch_to_thread(work_thread->kernelStack, 0);
	}




	extern "C" void nx_x64_find_and_switch_thread(uint64_t stackPointer)
	{
		// save the current stack.
		CurrentThread->kernelStack = stackPointer;

		if(CurrentThread->state == ThreadState::Running)
			ThreadList.append(CurrentThread);

		// else we blocked or smth, don't put it back in the runqueue.

		CurrentThread = getNextThread();
		CurrentThread->state = ThreadState::Running;

		// println("(switch %ld) (%p)", next->threadId, next->kernelStack);
		nx_x64_switch_to_thread(CurrentThread->kernelStack, 0);
	}


	void yield()
	{
		asm volatile ("int $0xF0");
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
			if(thr->blockedMtx == mtx)
			{
				thr->blockedMtx = 0;
				thr->state = ThreadState::Stopped;

				ThreadList.prepend(thr);
			}
		}
	}


	Thread* getNextThread()
	{
		return ThreadList.popFront();
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




























