// scheduler.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

#include "devices/pc/apic.h"
#include "devices/pc/pit8253.h"

extern "C" void nx_x64_yield_thread();
extern "C" void nx_x64_tick_handler();
extern "C" void nx_x64_switch_to_thread(uint64_t stackPtr, uint64_t cr3, void* fpuState, nx::scheduler::Thread* thr);
extern "C" void nx_x64_clear_segments();

extern "C" uint8_t nx_x64_is_in_syscall;
extern "C" uint8_t nx_user_kernel_stubs_begin;
extern "C" uint8_t nx_user_kernel_stubs_end;

// actually, the parameters are not important, we just need its address.
extern "C" void nx_x64_user_signal_enter(uint64_t sender, uint64_t sigType, uint64_t a, uint64_t b, uint64_t c);

extern "C" void idle_worker();

namespace nx {
namespace scheduler
{
	constexpr uint64_t NS_PER_TICK           = time::milliseconds(1).ns();
	constexpr uint64_t TIMESLICE_DURATION_NS = time::milliseconds(4).ns();

	static_assert(NS_PER_TICK <= TIMESLICE_DURATION_NS);
	static_assert(TIMESLICE_DURATION_NS % NS_PER_TICK == 0);    // not strictly necessary

	static void calibrateTickTimer();

	static Thread* getNextThread(State* ss)
	{
		// no point sorting, cos the list might change frequently.
		// just linear search to find the thread with the highest priority.
		Thread* next = 0;
		Priority max;
		for(auto thr : ss->ThreadList)
		{
			if(thr->state == ThreadState::Stopped && (!next || thr->priority > max))
				next = thr, max = thr->priority;
		}

		if(!next)
			return ss->IdleThread;

		assert(next);
		return next;
	}

	static int __nested_sched = 0;
	extern "C" void nx_x64_find_and_switch_thread(uint64_t stackPointer)
	{
		assert(!__nested_sched && "TICK PREEMPTED?!");
		__nested_sched = 1;

		auto ss = getSchedState();

		Thread* oldthr = ss->CurrentThread;
		assert(oldthr);

		// save the current stack.
		oldthr->kernelStack = stackPointer;

		if(oldthr->state == ThreadState::Running)
			oldthr->state = ThreadState::Stopped;

		// save the fpu state
		cpu::fpu::save(oldthr->fpuSavedStateBuffer);

		Thread* newthr = getNextThread(ss);
		newthr->state = ThreadState::Running;

		// reset the priority, since it just got scheduled
		newthr->priority.reset();

		cpu::tss::setRSP0(getCPULocalState()->TSSBase, newthr->kernelStackTop);
		cpu::tss::updateIOPB(getCPULocalState()->TSSBase, newthr->parent->ioPorts);

		// note wrt the selectors:
		// on intel, writing 0 to %fs clears FSBase. so we must save the current fsbase,
		// zero the segments, then write the new fsbase.
		oldthr->fsBase = cpu::readFSBase();

		// clear the selectors
		nx_x64_clear_segments();

		// load the new fsbase
		cpu::writeFSBase(newthr->fsBase);

		auto oldcr3 = oldthr->parent->addrspace.cr3;
		auto newcr3 = newthr->parent->addrspace.cr3;

		ss->CurrentThread = newthr;

		getCPULocalState()->currentThread = newthr;
		getCPULocalState()->cpu->currentProcess = newthr->parent;

		// serial::debugprintf("old: [t=%lu, p=%lu, cr3=%p, sp=%p]\n", oldthr->threadId, oldthr->parent->processId,
		// 	oldthr->parent->addrspace.cr3.addr(), stackPointer);
		// serial::debugprintf("new: [t=%lu, p=%lu, cr3=%p, sp=%p]\n", newthr->threadId, newthr->parent->processId,
		// 	newcr3.addr(), newthr->kernelStack);

		__nested_sched = 0;
		nx_x64_switch_to_thread(newthr->kernelStack, newcr3 == oldcr3 ? 0 : newcr3.addr(),
			(void*) newthr->fpuSavedStateBuffer, newthr);
	}

	extern "C" void nx_x64_setup_signalled_stack(Thread* newthr)
	{
		// if the thread is pending a restore, we must perform reverse reconstructive surgery.
		// we just restore the entire saved state instead of fiddling with individual things.
		if(newthr->pendingSignalRestore)
		{
			assert(!newthr->savedSignalStateStack.empty());
			auto savedstate = newthr->savedSignalStateStack.popBack();

			auto intstate = (cpu::InterruptedState*) newthr->kernelStack;
			memcpy(intstate, &savedstate, sizeof(savedstate));

			newthr->pendingSignalRestore = false;
			// log("sched", "restoring thread %lu (%zu) -> %p", newthr->threadId, newthr->savedSignalStateStack.size(),
			// 	intstate->rip);

			// intstate->dump();
			// serial::debugprint("\n");

			return;
		}

		// check if we have pending signals.
		if(newthr->pendingSignalQueue.empty())
			return;

		// first, save the state.
		auto regs = (cpu::InterruptedState*) newthr->kernelStack;

		// if the thread is currently sitting in kernel mode, we cannot signal it.
		// note: this means that, for now, we cannot send signals to kernel threads!
		if((regs->cs & 0x3) == 0)
		{
			// just quit -- don't modify anything, and wait till the next schedule event
			// to potentially signal the thread.
			return;
		}

		// ok, we're in user mode here.
		newthr->savedSignalStateStack.append(*regs);

		auto msg = newthr->pendingSignalQueue.popFront();
		// the signature: nx_x64_user_signal_enter
		// (uint64_t sender, uint64_t sigType, uint64_t a, uint64_t b, uint64_t c, uint64_t handler);
		//    ^ rdi            ^ rsi             ^ rdx       ^ rcx       ^ r8        ^ r9

		// try not to leak info to anybody.
		regs->rax = 0;
		regs->rbx = 0;
		regs->r10 = 0;
		regs->r11 = 0;
		regs->r12 = 0;
		regs->r13 = 0;
		regs->r14 = 0;
		regs->r15 = 0;
		regs->rbp = 0;

		// perform precision surgery
		regs->rdi = msg.senderId;
		regs->rsi = msg.body.sigType;
		regs->rdx = msg.body.a;
		regs->rcx = msg.body.b;
		regs->r8  = msg.body.c;

		assert(msg.body.sigType < ipc::MAX_SIGNAL_TYPES);
		regs->r9  = (uintptr_t) newthr->signalHandlers[msg.body.sigType];

		// fixup the rip:
		regs->rip = addrs::USER_KERNEL_STUBS.addr() + ((uintptr_t) nx_x64_user_signal_enter - (uintptr_t) &nx_user_kernel_stubs_begin);

		// surgery ok.
		// log("ipc", "sending signalled message (sigType %lu) to tid %lu / pid %lu",
		// 	msg.body.sigType, newthr->threadId, newthr->parent->processId);
	}





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

					log("sched", "destroying thread %lu", thr->threadId);

					ss->ThreadList.remove_all(thr);

					log("sched", "removed thread %lu", thr->threadId);
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

		nx_x64_switch_to_thread(ss->IdleThread->kernelStack, 0, (void*) ss->IdleThread->fpuSavedStateBuffer, ss->CurrentThread);
	}





	void yield()
	{
		asm volatile ("int $0xF0");
	}





	void exit(int status)
	{
		auto ss = getSchedState();

		auto t = getCurrentThread();
		assert(t);

		LockedSection(&ss->lock, [&]() {
			t->state = ThreadState::AboutToExit;
			ss->DestructionQueue.append(t);

			log("sched", "exiting thread %lu (status = %d)", t->threadId, status);
		});

		yield();

		while(true);
	}

	void terminate(Thread* t)
	{
		assert(t);

		auto ss = getSchedState();
		LockedSection(&ss->lock, [&]() {

			t->state = ThreadState::AboutToExit;
			ss->DestructionQueue.append(t);

			log("sched", "terminating tid %lu", t->threadId);
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

			log("sched", "terminating process %lu", p->processId);
		});

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

		auto ss = getSchedState();

		LockedSection(&ss->lock, [&]() {
			ss->BlockedThreads.append(t);

			t->blockedMtx = mtx;
			t->state = ThreadState::BlockedOnMutex;

		});

		yield();
	}

	void unblock(mutex* mtx)
	{
		// note: we allow you to call unblock before the scheduler is up,
		// because we unconditionally call unblock() when releasing a mutex,
		// and we use mutexes in code that is called during early-init.
		if(getInitPhase() < SchedulerInitPhase::SchedulerStarted)
			return;

		// hmm.
		auto& bthrs = getSchedState()->BlockedThreads;

		for(auto thr = bthrs.begin(); thr != bthrs.end(); )
		{
			if((*thr)->state == ThreadState::BlockedOnMutex && (*thr)->blockedMtx == mtx)
			{
				(*thr)->blockedMtx = 0;
				(*thr)->state = ThreadState::Stopped;

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

		LockedSection(&getSchedState()->lock, [&]() {
			t->state = ThreadState::BlockedOnSleep;
			t->wakeUpTimestamp = getElapsedNanoseconds() + ns;

			getSchedState()->BlockedThreads.append(t);
		});

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

				woke = true;

				// now, you ask: why is this an array? because we *DO NOT* want to touch any heap locks
				// inside the scheduler. removing stuff from an array never touches the heap (only memmove),
				// while removing things from a linked list requires deleting the node.

				// since we never *add* to the blocked thread array while inside a critical section, we can
				// guarantee that we'll never need the heap here.
				thr = bthrs.erase(thr);
			}
			else
			{
				++thr;
			}
		}

		return woke;
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












	// tick-related stuff

	static int TickIRQ = 0;
	void setTickIRQ(int irq)
	{
		TickIRQ = irq;
	}

	int getTickIRQ()
	{
		return TickIRQ;
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
	// note: this is called from asm-land

	static int __nested_tick = 0;
	extern "C" uint64_t nx_x64_scheduler_tick()
	{
		assert(!__nested_tick && "TICK PREEMPTED?!");
		__nested_tick = 1;

		// serial::debugprint('>');

		auto ss = getSchedState();

		ss->tickCounter += 1;
		ElapsedNanoseconds += NS_PER_TICK;

		bool woke = wakeUpThreads();
		// serial::debugprint('}');

		// other people can "expedite" a schedule event if necessary by setting this flag.
		bool ret = woke || ss->expediteSchedule || (ss->tickCounter * NS_PER_TICK) >= TIMESLICE_DURATION_NS;
		ss->expediteSchedule = false;

		if(ret) ss->tickCounter = 0;

		interrupts::sendEOI(TickIRQ);

		// if the CPU is holding any locks, then we cannot preempt.
		if(getCPULocalState()->numHeldLocks > 0)
			ret = false;

		// if(ret) serial::debugprint("*");
		// else    serial::debugprint(".");

		__nested_tick = 0;
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




























