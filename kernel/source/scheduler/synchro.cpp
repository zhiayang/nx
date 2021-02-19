// synchro.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx
{
	/*
		TODO: make these locks ticket locks, so it will be more fair

		struct spinlock { uint64_t ticket = 0; uint64_t next = 0; }

		fn lock()
		{
			let thisTicket = atomic::fetch_incr(&this->next);
			while(atomic::load(&this->ticket) != thisTicket)
				asm volatile ("pause");
		}

		fn unlock()
		{
			atomic::incr(&this->ticket);
		}

		// the issue is how we can make this race-free with the irq lock.
		// anyway fairness is probably not that big of an issue until we get more threads... i hope.
	*/


	// check if the lock is held by the current thread in a race-free manner.
	static bool check_selflock(scheduler::Thread* holder)
	{
		if(!holder)
			return false;

		// prevent the situation where we get the current thread and store it in a register,
		// and then we get scheduled, leading to a stale comparison. (since obviously before
		// the lock is acquired, the scheduler is not paused).

		scheduler::pause();
		auto cur = scheduler::getCurrentThread();
		auto ret = (cur == holder);

		scheduler::unpause();
		return ret;
	}


	spinlock::spinlock() { }
	spinlock::spinlock(spinlock&& other)
	{
		autolock lk(&other);

		this->value  = 0;
		this->holder = 0;
	}

	bool spinlock::held()
	{
		return this->value;
	}

	void spinlock::lock()
	{
		if(check_selflock(this->holder))
		{
			error("spinlock", "trying to acquire lock held by self! (tid {}, pid {})",
				this->holder->threadId, this->holder->parent->processId);

			abort("deadlock");
		}

		// this will also help us atomically increment the number of held locks.
		atomic::cas_spinlock(&this->value, 0, 1);

		this->holder = scheduler::getCurrentThread();
	}

	void spinlock::unlock()
	{
		atomic::store(&this->value, 0);
		atomic::store(&this->holder, 0);

		// atomic::decr(&scheduler::getCPULocalState()->numHeldLocks);
		scheduler::unpause();
	}

	bool spinlock::trylock()
	{
		if(!atomic::cas_trylock(&this->value, 0, 1))
			return false;

		this->holder = scheduler::getCurrentThread();

		// atomic::incr(&scheduler::getCPULocalState()->numHeldLocks);
		// scheduler::pause();

		return true;
	}





	r_spinlock::r_spinlock() { }
	r_spinlock::r_spinlock(r_spinlock&& other)
	{
		autolock lk(&other);

		this->value  = 0;
		this->holder = 0;
		this->recursion = 0;
	}

	bool r_spinlock::held()
	{
		return this->value;
	}

	void r_spinlock::lock()
	{
		if(check_selflock(this->holder))
			return atomic::incr(&this->recursion);

		// this will also help us atomically increment the number of held locks.
		atomic::cas_spinlock(&this->value, 0, 1);

		this->holder = scheduler::getCurrentThread();
	}

	void r_spinlock::unlock()
	{
		// the scheduler is paused when locked, so there should be no race condition here.
		if(this->recursion > 0)
			return atomic::decr(&this->recursion);

		atomic::store(&this->value, 0);
		atomic::store(&this->holder, 0);
		atomic::store(&this->recursion, 0);

		// atomic::decr(&scheduler::getCPULocalState()->numHeldLocks);
		scheduler::unpause();
	}

	bool r_spinlock::trylock()
	{
		if(!atomic::cas_trylock(&this->value, 0, 1))
			return false;

		this->holder = scheduler::getCurrentThread();

		// atomic::incr(&scheduler::getCPULocalState()->numHeldLocks);
		return true;
	}











	IRQSpinlock::IRQSpinlock() { }
	IRQSpinlock::IRQSpinlock(IRQSpinlock&& other)
	{
		autolock lk(&other);

		this->value  = 0;
		this->holder = 0;
	}

	bool IRQSpinlock::held()
	{
		return this->value;
	}


	void IRQSpinlock::lock()
	{
		if(this->holder && this->holder == scheduler::getCurrentThread())
		{
			error("IRQSpinlock", "trying to acquire lock held by self! (tid {}, pid {})",
				this->holder->threadId, this->holder->parent->processId);

			abort("deadlock");
		}

		// this asm function (see lock.s) ensures there's no race condition between
		// acquiring the lock and disabling interrupts. it also increments the lock counter.
		atomic::cas_spinlock_noirq(&this->value, 0, 1);

		this->holder = scheduler::getCurrentThread();
	}


	void IRQSpinlock::unlock()
	{
		atomic::store(&this->value, 0);
		atomic::store(&this->holder, 0);

		interrupts::enable();
	}


	bool IRQSpinlock::trylock()
	{
		// again, this function in lock.s thing ensures there's no race condition.
		if(!atomic::cas_trylock_noirq(&this->value, 0, 1))
			return false;

		this->holder = scheduler::getCurrentThread();

		return true;
	}





















	mutex::mutex() { }
	mutex::mutex(mutex&& other)
	{
		autolock lk(&other);

		this->value  = 0;
		this->holder = 0;
	}

	void mutex::lock()
	{
		if(this->holder && this->holder == scheduler::getCurrentThread())
		{
			error("mutex", "trying to acquire lock held by self! (tid {}, pid {})",
				this->holder->threadId, this->holder->parent->processId);

			// TODO: detect this situation and kill the process if we can.
			abort("deadlock");
		}

		while(!atomic::cas(&this->value, 0, 1))
			scheduler::block(this);

		this->holder = scheduler::getCurrentThread();
	}


	void mutex::unlock()
	{
		atomic::store(&this->value, 0);
		atomic::store(&this->holder, 0);

		scheduler::notifyOne(this);
	}


	bool mutex::held()
	{
		return this->value;
	}


	bool mutex::trylock()
	{
		if(!atomic::cas(&this->value, 0, 1))
			return false;

		this->holder = scheduler::getCurrentThread();

		return true;
	}
}














