// synchro.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once
#include "defs.h"
#include "atomic.h"

#include <type_traits>

namespace nx
{
	struct condition;

	namespace scheduler
	{
		struct Thread;
		void block(condition* cv);
		void notifyOne(condition* cv);
		void notifyAll(condition* cv);
	}

	namespace interrupts
	{
		void enable();
		void disable();
	}

	struct condition
	{
		// owo...
		virtual ~condition() { }

		uint64_t dummy = 0;
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


	// this one does not disable interrupts, only delays
	// thread switching when locked.
	struct spinlock
	{
		spinlock();
		spinlock(spinlock&&);

		spinlock(const spinlock&) = delete;
		spinlock& operator = (spinlock&&) = delete;
		spinlock& operator = (const spinlock&) = delete;

		bool held();
		void lock();
		void unlock();
		bool trylock();

	private:
		uint64_t value = 0;
		scheduler::Thread* holder = 0;
	};

	// this is a spinlock that is recursive.
	struct r_spinlock
	{
		r_spinlock();
		r_spinlock(r_spinlock&&);

		r_spinlock(const r_spinlock&) = delete;
		r_spinlock& operator = (r_spinlock&&) = delete;
		r_spinlock& operator = (const r_spinlock&) = delete;

		bool held();
		void lock();
		void unlock();
		bool trylock();

	private:
		uint64_t value = 0;
		uint64_t recursion = 0;
		scheduler::Thread* holder = 0;
	};


	// this one will disable interrupts also.
	struct IRQSpinlock
	{
		IRQSpinlock();
		IRQSpinlock(IRQSpinlock&&);

		IRQSpinlock(const IRQSpinlock&) = delete;
		IRQSpinlock& operator = (IRQSpinlock&&) = delete;
		IRQSpinlock& operator = (const IRQSpinlock&) = delete;

		bool held();
		void lock();
		void unlock();
		bool trylock();

	private:
		uint64_t value = 0;
		scheduler::Thread* holder = 0;
	};

	struct mutex : condition
	{
		mutex();
		mutex(mutex&&);

		mutex(const mutex&) = delete;
		mutex& operator = (mutex&&) = delete;
		mutex& operator = (const mutex&) = delete;

		bool held();
		void lock();
		void unlock();
		bool trylock();

	private:
		uint64_t value = 0;
		scheduler::Thread* holder = 0;
	};


	template <typename T>
	struct condvar
	{
		condvar() : value() { }
		condvar(const T& initial) : value(initial) { }

		// hmm
		condvar(const condvar&) = delete;
		condvar& operator = (const condvar&) = delete;

		condvar(condvar&& oth) : value(oth.value) { }
		condvar& operator = (condvar&& oth)
		{
			if(this != &oth)
				this->value = krt::move(oth.value);

			return *this;
		}

		T get() const { return this->value; }

		void set_quiet(const T& x)
		{
			autolock lk(&this->mtx);
			this->value = x;
		}

		void set(const T& x)
		{
			this->set_quiet(x);
			this->notify_one();
		}

		// TODO: wait with timeout
		void wait(const T& x)
		{
			while(true)
			{
				{
					autolock lk(&this->mtx);
					if(this->value == x)
						break;
				}
				scheduler::block(&this->cond);
			}
		}

		void notify_one()
		{
			scheduler::notifyOne(&this->cond);
		}

		void notify_all()
		{
			scheduler::notifyAll(&this->cond);
		}

	private:
		T value;
		mutex mtx;
		condition cond;
	};

	template <typename T>
	struct semaphore
	{
		semaphore() : value(0) { }
		semaphore(T x) : value(x) { }

		void post(T num = 1)
		{
			{
				autolock lk(&this->mtx);
				this->value += num;
			}

			if(num > 1) scheduler::notifyAll(&this->cond);
			else        scheduler::notifyOne(&this->cond);
		}

		void post_if_waiting(T num = 1)
		{
			if(atomic::load(&this->waiters) > 0)
				this->post(num);
		}

		void wait()
		{
			{
				autolock lk(&this->mtx);
				if(this->value > 0)
				{
					this->value -= 1;
					return;
				}
			}


			atomic::incr(&this->waiters);
			scheduler::block(&this->cond);

			{
				autolock lk(&this->mtx);
				this->value -= 1;

				atomic::decr(&this->waiters);
			}
		}

		uint64_t num_waiters()
		{
			return atomic::load(&this->waiters);
		}

	private:
		T value = 0;
		uint64_t waiters = 0;
		condition cond;
		mutex mtx;
	};


	template <typename T>
	struct wait_queue
	{
		wait_queue() : sem(0) { }

		void push(T x)
		{
			{
				autolock lk(&this->mtx);
				this->queue.append(krt::move(x));
			}
			this->sem.post();
		}

		void push_if_waiting(T x)
		{
			if(this->sem.num_waiters() == 0)
				return;

			this->push(krt::move(x));
		}

		template <typename... Args>
		void emplace(Args&&... xs)
		{
			{
				autolock lk(&this->mtx);
				this->queue.emplace(krt::forward<Args>(xs)...);
			}
			this->sem.post();
		}

		void push_quiet(T x)
		{
			{
				autolock lk(&this->mtx);
				this->queue.append(krt::move(x));
			}

			atomic::incr(&this->pending_notifies);
		}

		template <typename... Args>
		void emplace_quiet(Args&&... xs)
		{
			{
				autolock lk(&this->mtx);
				this->queue.emplace_back(krt::forward<Args>(xs)...);
			}

			atomic::incr(&this->pending_notifies);
		}

		void notify_pending()
		{
			auto tmp = atomic::store(&this->pending_notifies, 0);
			this->sem.post(tmp);
		}

		T pop()
		{
			this->sem.wait();

			{
				autolock lk(&this->mtx);
				auto ret = krt::move(this->queue.front());
				this->queue.popFront();

				return ret;
			}
		}

		size_t size() const
		{
			return this->queue.size();
		}

	private:
		int64_t pending_notifies = 0;
		nx::list<T> queue;
		mutex mtx;                  // mtx is for protecting the queue during push/pop
		semaphore<uint64_t> sem;    // sem is for signalling when the queue has stuff (or not)
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
			if(this != &oth)
			{
				assert_not_held(this->lk);
				this->lk.~Lk();

				new (&this->lk) Lk(krt::move(assert_not_held(oth.lk)));
				this->value = krt::move(oth.value);
			}

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





















