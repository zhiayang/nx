// synchro.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx
{
	// bool __sync_bool_compare_and_swap (type *ptr, type oldval, type newval, ...)
	// These builtins perform an atomic compare and swap. That is, if the current value of *ptr is oldval, then write newval into *ptr.
	// The "bool" version returns true if the comparison is successful and newval was written.

	void spinlock::lock()
	{
		if(this->holder && this->holder == scheduler::getCurrentThread())
		{
			abort("trying to acquire lock held by self! (tid %lu, pid %lu)",
				this->holder->threadId, this->holder->parent->processId);
		}

		while(!__sync_bool_compare_and_swap(&this->value, 0, 1))
			asm volatile ("pause");

		this->holder = scheduler::getCurrentThread();
	}

	void spinlock::unlock()
	{
		this->value = 0;
		this->holder = 0;
	}

	bool spinlock::held()
	{
		return this->value;
	}

	bool spinlock::trylock()
	{
		return __sync_bool_compare_and_swap(&this->value, 0, 1);
	}




	void mutex::lock()
	{
		if(this->holder && this->holder == scheduler::getCurrentThread())
		{
			abort("trying to acquire lock held by self! (tid %lu, pid %lu)",
				this->holder->threadId, this->holder->parent->processId);
		}

		while(!__sync_bool_compare_and_swap(&this->value, 0, 1))
			scheduler::block(this);

		this->holder = scheduler::getCurrentThread();
	}

	void mutex::unlock()
	{
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
		return __sync_bool_compare_and_swap(&this->value, 0, 1);
	}
}














