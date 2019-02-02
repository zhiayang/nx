// synchro.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once
#include "defs.h"

namespace nx
{
	struct spinlock
	{
		bool test();
		void lock();
		void unlock();

		private:
		volatile uint8_t value = 0;
	};

	struct mutex
	{
		bool test();
		void lock();
		void unlock();

		private:
		volatile uint8_t value = 0;
	};

	struct semaphore
	{
	};



	template <typename T>
	struct autolock
	{
		autolock(T* x) : lk(x)  { this->lk->lock(); }
		~autolock()             { this->lk->unlock(); }

		autolock(const autolock&) = delete;
		autolock(const autolock&&) = delete;

		autolock& operator= (const autolock&) = delete;
		autolock& operator= (const autolock&&) = delete;

		private:
		T* lk = 0;
	};

}




















