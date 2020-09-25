// result.h
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <assert.h>
#include <type_traits>

namespace zst
{
	template <typename T, typename E>
	struct Result
	{
	private:
		static constexpr int STATE_NONE = 0;
		static constexpr int STATE_VAL  = 1;
		static constexpr int STATE_ERR  = 2;

		struct tag_err { };
		struct tag_ok { };

		Result(tag_ok _, const T& x)  : state(STATE_VAL), val(x) { }
		Result(tag_ok _, T&& x)       : state(STATE_VAL), val(static_cast<T&&>(x)) { }

		Result(tag_err _, const E& e) : state(STATE_ERR), err(e) { }
		Result(tag_err _, E&& e)      : state(STATE_ERR), err(static_cast<E&&>(e)) { }

	public:

		using value_type = T;
		using error_type = E;

		~Result()
		{
			if(state == STATE_VAL) this->val.~T();
			if(state == STATE_ERR) this->err.~E();
		}

		Result(const T& x) : Result(tag_ok(), x) { }
		Result(T&& x)      : Result(tag_ok(), static_cast<T&&>(x)) { }

		Result(const E& e) : Result(tag_err(), e) { }
		Result(E&& e)      : Result(tag_err(), static_cast<E&&>(e)) { }

		Result(const Result& other)
		{
			this->state = other.state;
			if(this->state == STATE_VAL) new(&this->val) T(other.val);
			if(this->state == STATE_ERR) new(&this->err) E(other.err);
		}

		Result(Result&& other)
		{
			this->state = other.state;
			other.state = STATE_NONE;

			if(this->state == STATE_VAL) new(&this->val) T(static_cast<T&&>(other.val));
			if(this->state == STATE_ERR) new(&this->err) E(static_cast<E&&>(other.err));
		}

		Result& operator=(const Result& other)
		{
			if(this != &other)
			{
				if(this->state == STATE_VAL) this->val.~T();
				if(this->state == STATE_ERR) this->err.~E();

				this->state = other.state;
				if(this->state == STATE_VAL) new(&this->val) T(other.val);
				if(this->state == STATE_ERR) new(&this->err) E(other.err);
			}
			return *this;
		}

		Result& operator=(Result&& other)
		{
			if(this != &other)
			{
				if(this->state == STATE_VAL) this->val.~T();
				if(this->state == STATE_ERR) this->err.~E();

				this->state = other.state;
				other.state = STATE_NONE;

				if(this->state == STATE_VAL) new(&this->val) T(static_cast<T&&>(other.val));
				if(this->state == STATE_ERR) new(&this->err) E(static_cast<E&&>(other.err));
			}
			return *this;
		}

		T* operator -> () { assert(this->state == STATE_VAL); return &this->val; }
		const T* operator -> () const { assert(this->state == STATE_VAL); return &this->val; }

		operator bool() const { return this->state == STATE_VAL; }
		bool ok() const { return this->state == STATE_VAL; }

		const T& unwrap() const { assert(this->state == STATE_VAL); return this->val; }
		const E& error() const { assert(this->state == STATE_ERR); return this->err; }

		T& unwrap() { assert(this->state == STATE_VAL); return this->val; }
		E& error() { assert(this->state == STATE_ERR); return this->err; }

		template <typename T1 = T>
		static Result of_value(T1&& val)
		{
			return Result<T, E>(tag_ok(), T(static_cast<T1&&>(val)));
		}

		template <typename... Args>
		static Result of_value(Args&&... xs)
		{
			return Result<T, E>(tag_ok(), T(static_cast<Args&&>(xs)...));
		}

		template <typename E1 = E>
		static Result of_error(E1&& err)
		{
			return Result<T, E>(tag_err(), E(static_cast<E1&&>(err)));
		}

		template <typename... Args>
		static Result of_error(Args&&... xs)
		{
			return Result<T, E>(tag_err(), E(static_cast<Args&&>(xs)...));
		}

		// enable implicit upcast to a base type
		template <typename U, typename = std::enable_if_t<
			std::is_pointer_v<T> && std::is_pointer_v<U>
			&& std::is_base_of_v<std::remove_pointer_t<U>, std::remove_pointer_t<T>>
		>>
		operator Result<U, E> () const
		{
			if(state == STATE_VAL)  return Result<U, E>(this->val);
			if(state == STATE_ERR)  return Result<U, E>(this->err);

			abort();
		}

	private:
		// 0 = schrodinger -- no error, no value.
		// 1 = valid
		// 2 = error
		int state = 0;
		union {
			T val;
			E err;
		};
	};


	template <typename E>
	struct Result<void, E>
	{
	private:

		static constexpr int STATE_NONE = 0;
		static constexpr int STATE_VAL  = 1;
		static constexpr int STATE_ERR  = 2;

	public:
		using value_type = void;
		using error_type = E;

		Result() : state(STATE_VAL) { }

		Result(const E& e) : state(STATE_ERR), err(e) {}
		Result(E&& e)      : state(STATE_ERR), err(static_cast<E&&>(e)) {}

		Result(const Result& other)
		{
			this->state = other.state;
			if(this->state == STATE_ERR)
				this->err = other.err;
		}

		Result(Result&& other)
		{
			this->state = other.state;
			other.state = STATE_NONE;

			if(this->state == STATE_ERR)
				this->err = static_cast<E&&>(other.err);
		}

		Result& operator=(const Result& other)
		{
			if(this != &other)
			{
				this->state = other.state;
				if(this->state == STATE_ERR)
					this->err = other.err;
			}
			return *this;
		}

		Result& operator=(Result&& other)
		{
			if(this != &other)
			{
				this->state = other.state;
				other.state = STATE_NONE;

				if(this->state == STATE_ERR)
					this->err = static_cast<E&&>(other.err);
			}
			return *this;
		}

		operator bool() const { return this->state == STATE_VAL; }
		bool ok() const { return this->state == STATE_VAL; }

		const E& error() const { assert(this->state == STATE_ERR); return this->err; }
		E& error() { assert(this->state == STATE_ERR); return this->err; }

		static Result of_value()
		{
			return Result<void, E>();
		}

		template <typename E1 = E>
		static Result of_error(E1&& err)
		{
			return Result<void, E>(E(static_cast<E1&&>(err)));
		}

		template <typename... Args>
		static Result of_error(Args&&... xs)
		{
			return Result<void, E>(E(static_cast<Args&&>(xs)...));
		}

	private:
		int state = 0;
		E err;
	};
}
