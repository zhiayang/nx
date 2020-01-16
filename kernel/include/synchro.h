// synchro.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once
#include "defs.h"
#include <type_traits>

namespace nx
{
	namespace scheduler
	{
		struct Thread;
	}

	namespace interrupts
	{
		void enable();
		void disable();
	}

	struct spinlock
	{
		bool held();
		void lock();
		void unlock();
		bool trylock();

	private:
		volatile uint8_t value = 0;
		volatile scheduler::Thread* holder = 0;
	};

	struct mutex
	{
		bool held();
		void lock();
		void unlock();
		bool trylock();

	private:
		volatile uint8_t value = 0;
		volatile scheduler::Thread* holder = 0;
	};

	struct semaphore
	{
	};



	template <typename T>
	struct autolock
	{
		// note: we allow initialising with NULL, which is a no-op.
		autolock(T* x) : lk(x)  { if(this->lk) this->lk->lock(); }
		~autolock()             { if(this->lk) this->lk->unlock(); }

		autolock(const autolock&) = delete;
		autolock(autolock&&) = delete;

		autolock& operator= (const autolock&) = delete;
		autolock& operator= (autolock&&) = delete;

		private:
		T* lk = 0;
	};

	template <typename Lk, typename Functor>
	struct LockedSection
	{
		LockedSection(Lk* x, Functor&& f)
		{
			if(x)
			{
				this->lk = x;
				this->lk->lock();

				f();
			}
		}

		~LockedSection()
		{
			if(this->lk)
			{
				this->lk->unlock();
			}
		}

		LockedSection(const LockedSection&) = delete;
		LockedSection(LockedSection&&) = delete;

	private:
		Lk* lk = 0;
	};

	template <typename T>
	struct CriticalSection
	{
		CriticalSection(T&& x)
		{
			interrupts::disable();
			x();
		}

		~CriticalSection()
		{
			interrupts::enable();
		}

		CriticalSection(const CriticalSection&) = delete;
		CriticalSection(CriticalSection&&) = delete;

		CriticalSection& operator= (const CriticalSection&) = delete;
		CriticalSection& operator= (CriticalSection&&) = delete;
	};
}





















