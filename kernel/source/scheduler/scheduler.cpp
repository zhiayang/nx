// scheduler.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

#include "devices/ports.h"
#include "devices/pc/apic.h"
#include "devices/pc/pit8253.h"

extern "C" void nx_x64_yield_thread();
extern "C" void nx_x64_tick_handler();
extern "C" void nx_x64_clear_segments();
extern "C" void nx_x64_find_and_switch_thread(uint64_t stackPointer);
extern "C" void nx_x64_switch_to_thread(uint64_t stackPtr, uint64_t cr3, void* fpuState, nx::scheduler::Thread* thr);

extern "C" uint8_t nx_x64_is_in_syscall;
extern "C" uint8_t nx_user_kernel_stubs_begin;
extern "C" uint8_t nx_user_kernel_stubs_end;

// actually, the parameters are not important, we just need its address.
extern "C" void nx_x64_user_signal_enter(uint64_t sender, uint64_t sigType, uint64_t a, uint64_t b, uint64_t c);

extern "C" void idle_worker();

namespace nx {
namespace scheduler
{
	static void calibrateTickTimer();

	[[noreturn]] static void death_destroyer_of_threads(void* _ss)
	{
		auto ss = (State*) _ss;
		while(true)
		{
			while(ss->DestructionQueue.size() > 0)
			{
				Thread* thr = 0;

				LockedSection(&ss->lock, [&]() {
					thr = ss->DestructionQueue.popFront();
					assert(thr);

					ss->ThreadList.remove_all(thr);
				});

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

	// this should be called once per cpu
	void init()
	{
		calibrateTickTimer();

		auto ss = getSchedState();

		ss->IdleThread = createThread(getKernelProcess(), idle_worker);
		ss->CurrentThread = ss->IdleThread;

		ss->ThreadList = nx::list<Thread*>();
		ss->BlockedThreads = nx::array<Thread*>();
		ss->DestructionQueue = nx::list<Thread*>();

		ss->ProcessList = nx::list<Process*>();

		// add our own worker thread.
		addThread(createThread(getKernelProcess(), death_destroyer_of_threads, (void*) ss));

		log("sched", "scheduler initialised");
	}

	void installTickHandlers()
	{
		// install the tick handler
		cpu::idt::setEntry(IRQ_BASE_VECTOR + getTickIRQ(), (addr_t) nx_x64_tick_handler,
			/* cs: */ 0x08, /* ring3: */ false, /* nestable: */ false);

		// install the yield handler
		cpu::idt::setEntry(0xF0, (addr_t) nx_x64_yield_thread,
			/* cs: */ 0x08, /* ring3: */ false, /* nestable: */ false);
	}

	void start()
	{
		// the scheduler has started.
		setInitPhase(SchedulerInitPhase::SchedulerStarted);

		auto ss = getSchedState();
		ss->CurrentThread = ss->IdleThread;

		// primed & ready.
		interrupts::resetNesting();

		// sometimes we get stuck here, so maybe this will help...
		interrupts::sendEOI(getTickIRQ());

		nx_x64_switch_to_thread(ss->IdleThread->kernelStack, 0, (void*) ss->IdleThread->fpuSavedStateBuffer, ss->CurrentThread);
	}





	void exit(int status)
	{
		auto ss = getSchedState();
		assert(!platform::is_interrupted());

		auto t = getCurrentThread();
		assert(t);

		LockedSection(&ss->lock, [&]() {
			t->state = ThreadState::AboutToExit;
			ss->DestructionQueue.append(t);

			log("sched", "exiting thread {} (status = {})", t->threadId, status);
		});

		yield();
		while(true);
	}

	void crashFromException()
	{
		auto ss = getSchedState();

		auto thr = getCurrentThread();
		assert(thr);

		LockedSection(&ss->lock, [&]() {
			thr->state = ThreadState::AboutToExit;
			ss->DestructionQueue.append(thr);

			warn("kernel", "killing thread {} (from process {}) due to exception",
				thr->threadId, thr->parent->processId);
		});

		// here's the difference -- we need to switch now, but reset the interrupt level.
		assert(platform::is_interrupted());
		platform::leave_interrupt_context();

		// pass the dummy here.
		nx_x64_find_and_switch_thread(thr->kernelStack);

		// we never get here.
		while(true);
	}

	void terminate(Thread* t)
	{
		assert(t);

		auto ss = getSchedState();
		LockedSection(&ss->lock, [&]() {

			t->state = ThreadState::AboutToExit;
			ss->DestructionQueue.append(t);

			log("sched", "terminating tid {}", t->threadId);
		});

		// if you are terminating yourself, then we need to yield.
		if(getCurrentThread() == t)
		{
			yield();
			while(true);
		}
	}


	void terminate(Process* p)
	{
		assert(p);

		auto ss = getSchedState();

		LockedSection(&ss->lock, [&]() {
			// schedule all threads to die.
			for(auto& t : p->threads)
			{
				ss->DestructionQueue.append(&t);
				t.state = ThreadState::AboutToExit;
			}

			log("sched", "terminating process {}", p->processId);
		});

		// if you are terminating yourself, then we need to yield.
		if(getCurrentProcess() == p)
		{
			yield();
			while(true);
		}
	}


	void block(condition* cv)
	{
		auto t = getCurrentThread();
		assert(t);

		auto ss = getSchedState();

		LockedSection(&ss->lock, [&]() {
			ss->BlockedThreads.append(t);

			t->blockedOn = cv;
			t->state = ThreadState::BlockedOnCondition;

		});

		yield();
	}

	static void notify(condition* cv, bool all)
	{
		// note: we allow you to call unblock before the scheduler is up,
		// because we unconditionally call unblock() when releasing a mutex,
		// and we use mutexes in code that is called during early-init.
		if(getInitPhase() < SchedulerInitPhase::SchedulerStarted)
			return;

		auto ss = getSchedState();

		// hmm.
		LockedSection(&ss->lock, [&cv, &ss, &all]() {
			auto& bthrs = ss->BlockedThreads;

			// ugh, there should be better way to do this (eg. a hashmap)
			for(auto _thr = bthrs.begin(); _thr != bthrs.end(); )
			{
				auto thr = *_thr;
				assert(thr);

				// TODO
				//! owo
				// i think there's some sort of race condition where we dispose of the thread or something
				// while it is holding locks? this place causes a null pointer deref undeterministically.
				if(thr->state == ThreadState::BlockedOnCondition && thr->blockedOn == cv)
				{
					thr->blockedOn = 0;
					thr->state = ThreadState::Stopped;
					thr->priority.boost();

					_thr = bthrs.erase(_thr);

					if(!all)
						break;
				}
				else
				{
					_thr++;
				}
			}
		});
	}

	void notifyOne(condition* cv)
	{
		notify(cv, /* all: */ false);
	}

	void notifyAll(condition* cv)
	{
		notify(cv, /* all: */ true);
	}
















	void nanosleep(uint64_t ns)
	{
		auto t = getCurrentThread();
		assert(t);

		auto ss = getSchedState();
		LockedSection(&ss->lock, [&]() {
			t->state = ThreadState::BlockedOnSleep;
			t->wakeUpTimestamp = getElapsedNanoseconds() + ns;

			getSchedState()->BlockedThreads.append(t);
		});

		yield();
	}

	Thread* getCurrentThread()
	{
		if(getInitPhase() < SchedulerInitPhase::SchedulerStarted)
			return 0;

		return getSchedState()->CurrentThread;
	}

	Thread* addThread(Thread* t)
	{
		assert(t);
		getSchedState()->ThreadList.append(t);
		return t;
	}

	State* getSchedState()
	{
		return &getCPULocalState()->cpu->schedState;
	}

	static void calibrateTickTimer()
	{
		if(interrupts::hasLocalAPIC())
		{
			// setup stuff
			device::apic::initLAPIC();

			// calibrate the local apic timer for our scheduler ticks
			auto vec = device::apic::calibrateLAPICTimer();
			setTickIRQ(vec);
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




























