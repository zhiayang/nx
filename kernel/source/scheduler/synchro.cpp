// synchro.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx
{
	void spinlock::lock()
	{
		while(__sync_lock_test_and_set(&this->value, 1))
			; // asm volatile ("pause");
	}

	void spinlock::unlock()
	{
		__sync_lock_release(&this->value);
	}

	bool spinlock::test()
	{
		return this->value;
	}



	void mutex::lock()
	{
		while(__sync_lock_test_and_set(&this->value, 1))
			scheduler::block(this);
	}

	void mutex::unlock()
	{
		__sync_lock_release(&this->value);
		scheduler::unblock(this);
	}

	bool mutex::test()
	{
		return this->value;
	}
}