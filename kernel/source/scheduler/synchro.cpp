// synchro.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

extern "C" uint64_t nx_x64_get_rflags();

namespace nx
{
	// bool __sync_bool_compare_and_swap (type *ptr, type oldval, type newval, ...)
	// These builtins perform an atomic compare and swap. That is, if the current value of *ptr is oldval, then write newval into *ptr.
	// The "bool" version returns true if the comparison is successful and newval was written.

	using SchedState = scheduler::SchedulerInitPhase;

	spinlock::spinlock() { }

	void spinlock::lock()
	{
		if(this->holder && this->holder == scheduler::getCurrentThread())
		{
			abort("trying to acquire lock held by self! (tid %lu, pid %lu)",
				this->holder->threadId, this->holder->parent->processId);
		}

		while(!atomic::cas(&this->value, 0, 1))
			asm volatile ("pause");

		interrupts::disable();

		// if the scheduler hasn't started properly, don't try to touch cpu-local state
		// cos it doesn't exist.
		if(__likely(scheduler::getInitPhase() >= SchedState::SchedulerStarted))
		{
			this->holder = scheduler::getCurrentThread();
			// log("spin", "tid %lu held lock %p (%d)", this->holder->threadId, this, nx_x64_get_rflags());

			// this only needs to be relaxed, since there will be no other threads running.
			// (it's a per-cpu value)
			// __atomic_add_fetch(&scheduler::getCPULocalState()->numHeldLocks, 1, __ATOMIC_RELAXED);
		}
	}

	void spinlock::unlock()
	{
		// if the scheduler hasn't started properly, don't try to touch cpu-local state
		// cos it doesn't exist.
		if(__likely(scheduler::getInitPhase() >= SchedState::SchedulerStarted))
		{
			// log("spin", "unlock: %p (%x) (%d)", this, nx_x64_get_rflags(), this->value);
			// log("spin", "tid %lu released lock", this->holder->threadId);
			// __atomic_sub_fetch(&scheduler::getCPULocalState()->numHeldLocks, 1, __ATOMIC_RELAXED);
		}

		__atomic_store_n(&this->value, 0, __ATOMIC_SEQ_CST);
		__atomic_store_n(&this->holder, 0, __ATOMIC_SEQ_CST);

		interrupts::enable();
	}

	bool spinlock::held()
	{
		return this->value;
	}

	bool spinlock::trylock()
	{
		if(atomic::cas(&this->value, 0, 1))
		{
			interrupts::disable();

			// if the scheduler hasn't started properly, don't try to touch cpu-local state
			// cos it doesn't exist.
			if(__likely(scheduler::getInitPhase() >= SchedState::SchedulerStarted))
			{
				this->holder = scheduler::getCurrentThread();
				// log("spin", "tid %lu held lock %p (%d)", this->holder->threadId, this, nx_x64_get_rflags());

				// this only needs to be relaxed, since there will be no other threads running.
				// (it's a per-cpu value)
				// __atomic_add_fetch(&scheduler::getCPULocalState()->numHeldLocks, 1, __ATOMIC_RELAXED);
			}

			return true;
		}

		return false;
	}



	mutex::mutex() { }

	void mutex::lock()
	{
		if(this->holder && this->holder == scheduler::getCurrentThread())
		{
			abort("trying to acquire lock held by self! (tid %lu, pid %lu)",
				this->holder->threadId, this->holder->parent->processId);
		}

		while(!atomic::cas(&this->value, 0, 1))
			scheduler::block(this);

		this->holder = scheduler::getCurrentThread();
		// log("mutex", "tid %lu acquired %p", holder ? holder->threadId : 0, this);
	}

	void mutex::unlock()
	{
		// log("mutex", "tid %lu released %p", holder ? holder->threadId : 0, this);

		this->value = 0;
		this->holder = 0;

		scheduler::unblock(this);
	}

	bool mutex::held()
	{
		return this->value;
	}

	bool mutex::trylock()
	{
		if(atomic::cas(&this->value, 0, 1))
		{
			this->holder = scheduler::getCurrentThread();
			return true;
		}

		return false;
	}
}














