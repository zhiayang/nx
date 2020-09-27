// switch.cpp
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.

// deals with the actual thread context switching.

#include "nx.h"

extern "C" void nx_x64_yield_thread();
extern "C" void nx_x64_tick_handler();
extern "C" void nx_x64_switch_to_thread(uint64_t stackPtr, uint64_t cr3, void* fpuState, nx::scheduler::Thread* thr);
extern "C" void nx_x64_clear_segments();

extern "C" uint8_t nx_x64_is_in_syscall;
extern "C" uint8_t nx_user_kernel_stubs_begin;
extern "C" uint8_t nx_user_kernel_stubs_end;

// actually, the parameters are not important, we just need its address.
extern "C" void nx_x64_user_signal_enter(uint64_t sender, uint64_t sigType, uint64_t a, uint64_t b, uint64_t c);

namespace nx {
namespace scheduler
{
	static_assert(NS_PER_TICK <= TIMESLICE_DURATION_NS);
	static_assert(TIMESLICE_DURATION_NS % NS_PER_TICK == 0);    // not strictly necessary

	static Thread* getNextThread(State* ss)
	{
		// no point sorting, cos the list might change frequently.
		// just linear search to find the thread with the highest priority.
		Thread* next = 0;
		auto max = Priority::low();
		for(auto thr : ss->ThreadList)
		{
			if(thr->state == ThreadState::Stopped && (!next || thr->priority > max))
				next = thr, max = thr->priority;
		}

		if(!next) next = ss->IdleThread;

		// port::write1b(0x3F8, '[');
		// port::write1b(0x3F8, '0' + next->threadId);
		// port::write1b(0x3F8, ']');

		assert(next);
		return next;
	}

	static int __nested_sched = 0;
	extern "C" void nx_x64_find_and_switch_thread(uint64_t stackPointer)
	{
		auto ss = getSchedState();

		// this should never happen
		assert(!__nested_sched && "TICK PREEMPTED?!");
		__nested_sched = 1;

		// again, this should never happen; if our lock (or any lock for that matter) was acquired,
		// then the scheduler tick should *NOT* be returning true and switching threads.
		assert(!ss->lock.held() && "switching with held lock?!");

		Thread* oldthr = ss->CurrentThread;
		assert(oldthr);

		// save the current stack.
		oldthr->kernelStack = stackPointer;

		if(oldthr->state == ThreadState::Running)
			oldthr->state = ThreadState::Stopped;

		// save the fpu state
		cpu::fpu::save(oldthr->fpuSavedStateBuffer);

		Thread* newthr = getNextThread(ss);
		assert(newthr);

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

		auto oldcr3 = oldthr->parent->addrspace.cr3();
		auto newcr3 = newthr->parent->addrspace.cr3();

		ss->CurrentThread = newthr;

		getCPULocalState()->currentThread = newthr;
		getCPULocalState()->cpu->currentProcess = newthr->parent;

		// serial::debugprintf("old: [t={}, p={}, cr3={p}, sp={p}]\n", oldthr->threadId, oldthr->parent->processId,
		// 	oldthr->parent->addrspace.cr3(), stackPointer);
		// serial::debugprintf("new: [t={}, p={}, cr3={p}, sp={p}]\n", newthr->threadId, newthr->parent->processId,
		// 	newcr3, newthr->kernelStack);

		// serial::debugprintf("{p}\n", newcr3);

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
			auto [ savedstate, blocking_cv ] = newthr->savedSignalStateStack.popBack();

			auto intstate = (cpu::InterruptedState*) newthr->kernelStack;
			memcpy(intstate, &savedstate, sizeof(savedstate));

			newthr->pendingSignalRestore = false;

			auto ss = getSchedState();
			assert(!ss->lock.held());

			// since the sched state lock is not held, we *should be* safe to call wakeUpBlockers, which
			// needs to acquire the lock to access the list of blocked threads.
			if(blocking_cv)
				blocking_cv->set(false);

			return;
		}

		// make sure it's not locked. if it's locked, we're fucked. it's an IRQSpinlock,
		// so we shouldn't be in the scheduler if the lock was held! so, since it's not
		// held, we are free to use unsafeGet to access the innards.
		assert(!newthr->pendingSignalQueue.getLock().held());

		// check if we have pending signals.
		if(newthr->pendingSignalQueue.unsafeGet()->empty())
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

		auto msg = newthr->pendingSignalQueue.unsafeGet()->popFront();

		// ok, we're in user mode here.
		newthr->savedSignalStateStack.append({ *regs, msg.blocking_cv });

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
		// log("ipc", "sending signalled message (sigType {}) to tid {} / pid {}",
		// 	msg.body.sigType, newthr->threadId, newthr->parent->processId);
	}

	static bool wakeUpThreads();

	static int TickIRQ = 0;

	// returns true if it's time to preempt somebody
	// note: this is called from asm-land

	static int __nested_tick = 0;
	extern "C" uint64_t nx_x64_scheduler_tick()
	{
		assert(!__nested_tick && "TICK PREEMPTED?!");
		__nested_tick = 1;

		// serial::debugprintchar('.');

		auto ss = getSchedState();

		ss->tickCounter += 1;
		ss->timesliceTicks += 1;

		bool woke = wakeUpThreads();

		// other people can "expedite" a schedule event if necessary by setting this flag.
		bool ret = woke || ss->expediteSchedule || (ss->timesliceTicks * NS_PER_TICK) >= TIMESLICE_DURATION_NS;
		ss->expediteSchedule = false;

		interrupts::sendEOI(TickIRQ);

		// if the CPU is holding any locks, then we cannot preempt.
		if(getCPULocalState()->numHeldLocks > 0)
			ret = false;

		if(ret)
			ss->timesliceTicks = 0;

		__nested_tick = 0;

		return ret;
	}

	void yield()
	{
		assert(!getSchedState()->lock.held() && "cannot yield while holding the scheduler lock!!");
		asm volatile ("int $0xF0");
	}

	void setTickIRQ(int irq)
	{
		TickIRQ = irq;
	}

	int getTickIRQ()
	{
		return TickIRQ;
	}

	uint64_t getElapsedNanoseconds()
	{
		return getSchedState()->tickCounter * NS_PER_TICK;
	}

	uint64_t getNanosecondsPerTick()
	{
		return NS_PER_TICK;
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
}
}
