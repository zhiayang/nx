// synchro.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once
#include "defs.h"
#include "atomic.h"

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
		spinlock();

		bool held();
		void lock();
		void unlock();
		bool trylock();

	private:
		uint8_t value = 0;
		scheduler::Thread* holder = 0;
	};

	struct mutex
	{
		mutex();

		bool held();
		void lock();
		void unlock();
		bool trylock();

	private:
		uint8_t value = 0;
		scheduler::Thread* holder = 0;
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

	template <typename T, bool> struct _wrapper { };
	template <typename T> struct _wrapper<T, true> { _wrapper(T&& v) : value(v) { } T value; };

	// https://stackoverflow.com/a/38540922
	template <typename Lk, typename Functor, typename Functor_Result = std::invoke_result_t<Functor>>
	struct LockedSection
	{
		template <typename R = Functor_Result, typename E = std::enable_if_t<std::is_void_v<R>>, typename Q = E>
		LockedSection(Lk* x, Functor&& fn)
		{
			this->lk = x;
			this->lk->lock();

			fn();
		}

		template <typename R = Functor_Result, typename E = std::enable_if_t<!std::is_void_v<R>>>
		LockedSection(Lk* x, Functor&& fn) : result((this->lk = x, this->lk->lock(), fn()))
		{
		}

		~LockedSection()
		{
			if(this->lk)
			{
				this->lk->unlock();
			}
		}

		operator std::invoke_result_t<Functor> () const
		{
			return result.value;
		}

		LockedSection(const LockedSection&) = delete;
		LockedSection(LockedSection&&) = delete;

	private:
		Lk* lk = 0;

		_wrapper<Functor_Result, !std::is_void_v<Functor_Result>> result;
	};

	template <typename T, typename Lk = nx::mutex>
	struct Synchronised
	{
	private:
		struct LockedInstance;

		Lk lk;
		T value;

	public:
		~Synchronised() { }

		template <typename = std::enable_if_t<std::is_default_constructible_v<T>>>
		Synchronised() { }

		template <typename = std::enable_if_t<std::is_copy_constructible_v<T>>>
		Synchronised(const T& x) : value(x) { }

		template <typename = std::enable_if_t<std::is_move_constructible_v<T>>>
		Synchronised(T&& x) : value(krt::move(x)) { }

		Synchronised(const Synchronised&) = delete;
		Synchronised& operator = (const Synchronised&) = delete;


		Synchronised(Synchronised&& oth) : lk(krt::move(assert_not_held(oth.lk))), value(krt::move(oth.value)) { }
		Synchronised& operator = (Synchronised&& oth)
		{
			this->lk    = krt::move(assert_not_held(oth.lk));
			this->value = krt::move(oth.value);

			return *this;
		}

		bool isLocked() { return this->lk.held(); }

		template <typename Functor>
		auto map(Functor&& fn) -> decltype(fn(this->value))
		{
			autolock autolk(&this->lk);
			return fn(this->value);
		}

		template <typename Functor>
		void perform(Functor&& fn)
		{
			autolock autolk(&this->lk);
			fn(this->value);
		}


		LockedInstance lock()
		{
			return LockedInstance(*this);
		}

		T* get()
		{
			if(!this->lk.held())
				abort("cannot get while not locked!");

			return &this->value;
		}

		// unsafe!
		T* unsafeGet()
		{
			return &this->value;
		}

		Lk& getLock()
		{
			return this->lk;
		}

	private:

		static Lk& assert_not_held(Lk& lk) { if(lk.held()) abort("cannot move held Synchronised"); return lk; }


		struct LockedInstance
		{
			T* operator -> () { return &this->sync.value; }
			~LockedInstance() { this->sync.lk.unlock(); }

		private:
			LockedInstance(Synchronised& sync) : sync(sync) { this->sync.lk.lock(); }

			LockedInstance(LockedInstance&&) = delete;
			LockedInstance(const LockedInstance&) = delete;

			LockedInstance& operator = (LockedInstance&&) = delete;
			LockedInstance& operator = (const LockedInstance&) = delete;

			Synchronised& sync;

			friend struct Synchronised;
		};
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





















