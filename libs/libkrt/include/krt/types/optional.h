// optional.h
// Copyright (c) 2020, zhiayang, Apache License 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"
#include "string.h"

#include "krt/meta.h"

#include <type_traits>

namespace krt
{
	template <typename T, typename Al, typename Ab>
	struct optional;

	constexpr struct { } nullopt;

	namespace opt
	{
		template <typename U, typename Al, typename Ab, typename U1 = std::remove_reference_t<U>>
		optional<U1, Al, Ab> some(U&&);

		template <typename U, typename Al, typename Ab, typename U1 = std::remove_reference_t<U>>
		optional<U1, Al, Ab> some(const U&);
	}

	template <typename T, typename allocator, typename aborter>
	struct optional
	{
		// template <typename E = std::enable_if_t<std::is_copy_constructible_v<T>>>
		optional(const optional& other)
		{
			this->valid = other.valid;
			new (&this->x.val) T(other.x.val);
		}

		// template <typename E = std::enable_if_t<std::is_copy_constructible_v<T>>>
		optional& operator = (const optional& other)
		{
			if(this != &other)  return *this = optional(other);
			else                return *this;
		}

		optional(optional&& other)
		{
			this->valid = other.valid; other.valid = false;
			new (&this->x.val) T(krt::move(other.x.val));
		}

		// move assign
		optional& operator = (optional&& other)
		{
			if(this != &other)
			{
				if(this->valid)
					this->x.val.~T();

				this->valid = other.valid; other.valid = false;
				new (&this->x.val) T(krt::move(other.x.val));
			}

			return *this;
		}

		optional& operator = (decltype(nullopt) _)
		{
			if(this->valid)
				this->x.val.~T();

			this->valid = false;
		}

		// this must be accessible for implicit conversion.
		optional(decltype(nullopt) _) : valid(false) { }

		~optional()
		{
			if(this->valid)
				this->x.val.~T();
		}



		bool present() const { return this->valid; }
		bool empty() const { return !present(); }

		T get()
		{
			if(!this->valid)
				aborter::abort("optional::get(): no value!");

			return x.val;
		}


	private:
		explicit optional(T&& val)
		{
			new (&this->x.val) T(krt::move(val));
			this->valid = true;
		}

		template <typename E = std::enable_if_t<std::is_copy_constructible_v<T>>>
		explicit optional(const T& val)
		{
			new (&this->x.val) T(val);
			this->valid = true;
		}

		optional() : valid(false) { }

		union __wrapper {
			T val;
			uint8_t dummy;

			__wrapper() : dummy(0) { }
		} x;

		bool valid;


		template <typename U, typename Al, typename Ab, typename U1> friend optional<U1, Al, Ab> krt::opt::some(U&&);
		template <typename U, typename Al, typename Ab, typename U1> friend optional<U1, Al, Ab> krt::opt::some(const U&);
	};


	namespace opt
	{
		template <typename U, typename Al, typename Ab, typename U1 = std::remove_reference_t<U>>
		optional<U1, Al, Ab> some(U&& x)
		{
			return optional<U1, Al, Ab>(move(x));
		}

		template <typename U, typename Al, typename Ab, typename U1 = std::remove_reference_t<U>>
		optional<U1, Al, Ab> some(const U& x)
		{
			return optional<U1, Al, Ab>(x);
		}
	}
}
