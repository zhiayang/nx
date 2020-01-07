// scheduler.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

#include "devices/pc/apic.h"
#include "devices/pc/pit8253.h"

extern "C" void nx_x64_yield_thread();
extern "C" void nx_x64_tick_handler();
extern "C" void nx_x64_switch_to_thread(uint64_t stackPtr, uint64_t cr3, void* fpuState);

namespace nx {
namespace scheduler
{
	constexpr uint64_t NS_PER_TICK           = time::milliseconds(5).ns();
	constexpr uint64_t TIMESLICE_DURATION_NS = time::milliseconds(15).ns();

	static_assert(NS_PER_TICK <= TIMESLICE_DURATION_NS);
	static_assert(TIMESLICE_DURATION_NS % NS_PER_TICK == 0);    // not strictly necessary

	static void calibrateTickTimer();

	static Thread* getNextThread(State* ss)
	{
		if(interrupts::hasPendingIRQs())
			return interrupts::getHandlerThread();

		if(ss->ThreadList.empty())  return ss->IdleThread;
		else                        return ss->ThreadList.popFront();
	}

	extern "C" void nx_x64_find_and_switch_thread(uint64_t stackPointer)
	{
		auto ss = getSchedState();

		Thread* oldthr = ss->CurrentThread;

		// save the current stack.
		oldthr->kernelStack = stackPointer;

		// save the fpu state
		cpu::fpu::save(oldthr->fpuSavedStateBuffer);

		// only put it back in the runqueue if we didn't block or sleep
		if(oldthr->state == ThreadState::Running)
			ss->ThreadList.append(oldthr);

		Thread* newthr = getNextThread(ss);
		newthr->state = ThreadState::Running;

		cpu::tss::setRSP0(getCPULocalState()->TSSBase, newthr->kernelStackTop);
		cpu::tss::updateIOPB(getCPULocalState()->TSSBase, newthr->parent->ioPorts);

		// note wrt the selectors:
		// on intel, writing 0 to %fs clears FSBase. so we must save the current fsbase,
		// zero the segments, then write the new fsbase.
		oldthr->fsBase = cpu::readFSBase();

		// clear the selectors
		asm volatile ("mov $0, %%ax; mov %%ax, %%ds; mov %%ax, %%es" ::: "%eax");

		// load the new fsbase
		cpu::writeFSBase(newthr->fsBase);

		auto oldcr3 = oldthr->parent->cr3;
		auto newcr3 = newthr->parent->cr3;

		ss->CurrentThread = newthr;
		getCPULocalState()->cpu->currentProcess = newthr->parent;

		// log("sched", "-> %lu", newthr->parent->processId);
		nx_x64_switch_to_thread(newthr->kernelStack, newcr3 == oldcr3 ? 0 : newcr3, (void*) newthr->fpuSavedStateBuffer);
	}







	[[noreturn]] static void death_destroyer_of_threads(void* _ss)
	{
		auto ss = (State*) _ss;
		while(true)
		{
			while(ss->DestructionQueue.size() > 0)
			{
				auto thr = ss->DestructionQueue.popFront();
				assert(thr);

				destroyThread(thr);
			}

			yield();
		}
	}








	// this should be called once.
	void bootstrap()
	{
		// initialise the managed GDT, so we can allocate TSSes for the cpus.
		cpu::gdt::init();

		// init all cpus with their cpu-local storage, and give them each a TSS
		for(auto& cpu : getAllCPUs())
			initCPU(&cpu);
	}

	[[noreturn]] static void idle_worker() { while(true) asm volatile ("hlt"); }

	// this should be called once per cpu
	void init()
	{
		calibrateTickTimer();

		auto ss = getSchedState();

		ss->IdleThread = createThread(getKernelProcess(), idle_worker);

		ss->ThreadList = nx::list<Thread*>();
		ss->BlockedThreads = nx::list<Thread*>();
		ss->DestructionQueue = nx::list<Thread*>();

		ss->ProcessList = nx::list<Process*>();

		// add our own worker thread.
		addThread(createThread(getKernelProcess(), death_destroyer_of_threads, (void*) ss));

		log("sched", "scheduler initialised");
	}

	void installTickHandlers()
	{
		// install the tick handler
		cpu::idt::setEntry(IRQ_BASE_VECTOR + 0, (addr_t) nx_x64_tick_handler,
			/* cs: */ 0x08, /* ring3: */ false, /* nestable:  */ false);

		// install the yield handler
		cpu::idt::setEntry(0xF0, (addr_t) nx_x64_yield_thread,
			/* cs: */ 0x08, /* ring3: */ false, /* nestable:  */ false);
	}

	void start()
	{
		// the scheduler has started.
		setInitPhase(SchedulerInitPhase::SchedulerStarted);


		auto ss = getSchedState();

		ss->CurrentThread = ss->IdleThread;
		nx_x64_switch_to_thread(ss->IdleThread->kernelStack, 0, (void*) ss->IdleThread->fpuSavedStateBuffer);
	}





	void yield()
	{
		asm volatile ("int $0xF0");
	}

	void exit(int status)
	{
		auto t = getCurrentThread();
		assert(t);

		log("sched", "exiting thread %lu (status = %d)", t->threadId, status);

		t->state = ThreadState::AboutToExit;
		getSchedState()->DestructionQueue.append(t);

		yield(); while(true);
	}

	void terminate(Process* p)
	{
		// schedule all threads to die.
		for(auto& t : p->threads)
		{
			t.state = ThreadState::AboutToExit;
			getSchedState()->DestructionQueue.append(&t);
		}

		log("sched", "terminating process %lu", p->processId);

		// if you are terminating yourself, then we need to yield.
		if(getCurrentProcess() == p)
		{
			yield();
			while(true);
		}
	}


	void block(mutex* mtx)
	{
		auto t = getCurrentThread();
		assert(t);

		t->blockedMtx = mtx;
		t->state = ThreadState::BlockedOnMutex;

		getSchedState()->BlockedThreads.append(t);

		yield();
	}

	void unblock(mutex* mtx)
	{
		// hmm.
		auto& bthrs = getSchedState()->BlockedThreads;

		for(auto thr = bthrs.begin(); thr != bthrs.end(); )
		{
			if((*thr)->state == ThreadState::BlockedOnMutex && (*thr)->blockedMtx == mtx)
			{
				(*thr)->blockedMtx = 0;
				(*thr)->state = ThreadState::Stopped;

				getSchedState()->ThreadList.prepend(*thr);

				thr = bthrs.erase(thr);
			}
			else
			{
				thr++;
			}
		}
	}


	void sleep(uint64_t ns)
	{
		auto t = getCurrentThread();
		assert(t);

		t->wakeUpTimestamp = getElapsedNanoseconds() + ns;
		t->state = ThreadState::BlockedOnSleep;

		getSchedState()->BlockedThreads.append(t);

		yield();
	}

	static bool wakeUpThreads()
	{
		bool woke = false;

		// checks for sleeping threads and wakes them up if necessary.
		// TODO: optimise this!!!! (eg. sort the threads by the wakeup timestamp)

		auto now = getElapsedNanoseconds();
		auto& bthrs = getSchedState()->BlockedThreads;
		for(auto thr = bthrs.begin(); thr != bthrs.end(); )
		{
			if((*thr)->state == ThreadState::BlockedOnSleep && now >= (*thr)->wakeUpTimestamp)
			{
				(*thr)->wakeUpTimestamp = 0;
				(*thr)->state = ThreadState::Stopped;

				getSchedState()->ThreadList.prepend(*thr);
				woke = true;

				thr = bthrs.erase(thr);
			}
			else
			{
				thr++;
			}
		}

		return woke;
	}


	Thread* getCurrentThread()
	{
		return getSchedState()->CurrentThread;
	}

	Thread* addThread(Thread* t)
	{
		getSchedState()->ThreadList.append(t);
		return t;
	}

	State* getSchedState()
	{
		return &getCPULocalState()->cpu->schedState;
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
	extern "C" uint64_t nx_x64_scheduler_tick()      // note: this is called from asm-land
	{
		interrupts::sendEOI(TickIRQ);

		auto ss = getSchedState();

		ss->tickCounter += 1;
		ElapsedNanoseconds += NS_PER_TICK;

		bool woke = wakeUpThreads();

		// if we have pending IRQs, then we will switch threads to the handler thread.
		bool ret = woke || interrupts::hasPendingIRQs() || (ss->tickCounter * NS_PER_TICK) >= TIMESLICE_DURATION_NS;

		if(ret) ss->tickCounter = 0;
		return ret;
	}










	static void calibrateTickTimer()
	{
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
	}








	static auto initPhase = SchedulerInitPhase::Uninitialised;
	SchedulerInitPhase getInitPhase()
	{
		return initPhase;
	}

	void setInitPhase(SchedulerInitPhase p)
	{
		assert(p >= initPhase);
		initPhase = p;
	}
}
}




























