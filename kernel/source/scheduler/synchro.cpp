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


	spinlock::spinlock() { }
	spinlock::spinlock(spinlock&& other)
	{
		other.lock();

		// it's already locked here, so we don't need to worry about atomicity... probably.
		this->value  = 0; other.value = 0;
		this->holder = 0; other.holder = 0;
	}

	bool spinlock::held()
	{
		return this->value;
	}

	void spinlock::lock()
	{
		if(this->holder && this->holder == scheduler::getCurrentThread())
		{
			error("spinlock", "trying to acquire lock held by self! (tid %lu, pid %lu)",
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

		atomic::decr(&scheduler::getCPULocalState()->numHeldLocks);
	}


	bool spinlock::trylock()
	{
		if(!atomic::cas_trylock(&this->value, 0, 1))
			return false;

		this->holder = scheduler::getCurrentThread();

		atomic::incr(&scheduler::getCPULocalState()->numHeldLocks);

		return true;
	}







	IRQSpinlock::IRQSpinlock() { }
	IRQSpinlock::IRQSpinlock(IRQSpinlock&& other)
	{
		other.lock();
		this->value  = 0; other.value = 0;
		this->holder = 0; other.holder = 0;
	}

	bool IRQSpinlock::held()
	{
		return this->value;
	}


	void IRQSpinlock::lock()
	{
		if(this->holder && this->holder == scheduler::getCurrentThread())
		{
			error("IRQSpinlock", "trying to acquire lock held by self! (tid %lu, pid %lu)",
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

		atomic::decr(&scheduler::getCPULocalState()->numHeldLocks);
		interrupts::enable();
	}


	bool IRQSpinlock::trylock()
	{
		// again, this function in lock.s thing ensures there's no race condition.
		if(!atomic::cas_trylock_noirq(&this->value, 0, 1))
			return false;

		this->holder = scheduler::getCurrentThread();

		atomic::incr(&scheduler::getCPULocalState()->numHeldLocks);

		return true;
	}





















	mutex::mutex() { }
	mutex::mutex(mutex&& other)
	{
		other.lock();
		this->value  = 0; other.value = 0;
		this->holder = 0; other.holder = 0;
	}

	void mutex::lock()
	{
		if(this->holder && this->holder == scheduler::getCurrentThread())
		{
			error("mutex", "trying to acquire lock held by self! (tid %lu, pid %lu)",
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














