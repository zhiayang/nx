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
	template <typename T, typename Ab>
	struct optional;

	constexpr struct { } nullopt;

	namespace opt
	{
		template <typename U, typename Ab, typename U1 = std::remove_reference_t<U>>
		optional<U1, Ab> some(U&&);

		template <typename U, typename Ab, typename U1 = std::remove_reference_t<U>>
		optional<U1, Ab> some(const U&);

		template <typename U, typename Ab, typename U1 = std::remove_reference_t<U>, typename... Args>
		optional<U1, Ab> some(Args&&...);
	}

	template <typename T, typename aborter>
	struct optional
	{
	private:
		using real_type = std::conditional_t<std::is_same_v<T, void>, int, T>;

		template <typename X>
		struct is_optional : std::false_type { };

		template <typename X>
		struct is_optional<optional<X, aborter>> : std::true_type { };


	public:
		// template <typename E = std::enable_if_t<std::is_copy_constructible_v<T>>>
		optional(const optional& other)
		{
			this->valid = other.valid;

			if constexpr (!std::is_same_v<T, void>)
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

			if constexpr (!std::is_same_v<T, void>)
				new (&this->x.val) T(krt::move(other.x.val));
		}

		// move assign
		optional& operator = (optional&& other)
		{
			if(this != &other)
			{
				if constexpr (!std::is_same_v<T, void>)
				{
					if(this->valid)
						this->x.val.~T();
				}

				this->valid = other.valid; other.valid = false;

				if constexpr (!std::is_same_v<T, void>)
					new (&this->x.val) T(krt::move(other.x.val));
			}

			return *this;
		}

		optional& operator = (decltype(nullopt) _)
		{
			if constexpr (!std::is_same_v<T, void>)
			{
				if(this->valid)
					this->x.val.~T();
			}

			this->valid = false;
		}

		// this must be accessible for implicit conversion.
		optional(decltype(nullopt) _) : valid(false) { }

		~optional()
		{
			if constexpr (!std::is_same_v<T, void>)
			{
				if(this->valid)
					this->x.val.~T();
			}
		}



		bool present() const { return this->valid; }
		bool empty() const { return !present(); }

		T get()
		{
			this->assert_present();
			return x.val;
		}

		T* operator-> ()             { this->assert_present(); return &this->x.val; }
		const T* operator-> () const { this->assert_present(); return &this->x.val; }

		template <typename Fn>
		auto map(Fn&& fn) -> optional<decltype(fn(std::declval<std::add_lvalue_reference_t<T>>())), aborter>
		{
			if(this->present())
			{
				if constexpr (!std::is_same_v<decltype(fn(this->x.val)), void>)
				{
					return optional<decltype(fn(this->x.val)), aborter>(fn(this->x.val));
				}
				else
				{
					fn(this->x.val);
					return optional<void, aborter>::valid_dummy();
				}
			}
			else
			{
				return nullopt;
			}
		}

		template <typename Fn,
			typename R = decltype(std::declval<Fn>()(std::declval<std::add_lvalue_reference_t<T>>())),
			typename E = std::enable_if_t<is_optional<R>::value>
		>
		auto flatmap(Fn&& fn) -> R
		{
			if(this->present())
				return fn(this->x.val);

			else
				return nullopt;
		}

		template <typename Fn>
		optional<T, aborter>& performIfEmpty(Fn&& fn)
		{
			if(this->empty())
				fn();

			return *this;
		}

		template <typename Fn>
		const optional<T, aborter>& performIfEmpty(Fn&& fn) const
		{
			if(this->empty())
				fn();

			return *this;
		}

		template <typename T1 = T, typename E = std::enable_if_t<!std::is_same_v<T1, void>>>
		T orElse(const T1& val) const
		{
			if(this->empty())   return val;
			else                return this->x.val;
		}


	private:
		void assert_present()
		{
			if(!this->valid)
				aborter::abort("optional::get(): no value!");
		}

		template <typename T1 = T,
			typename E = std::enable_if_t<!std::is_same_v<T1, void> && !is_optional<std::decay_t<T1>>::value>
		>
		explicit optional(T1&& val)
		{
			new (&this->x.val) T(krt::move(val));
			this->valid = true;
		}

		template <typename T1 = T>
		explicit optional(const T1& val)
		{
			new (&this->x.val) T(val);
			this->valid = true;
		}

		template <typename... Args>
		explicit optional(Args&&... xs)
		{
			new (&this->x.val) T(krt::forward<Args>(xs)...);
			this->valid = true;
		}

		optional() : valid(false) { }

		union __wrapper {
			real_type val;
			uint8_t dummy;

			__wrapper() : dummy(0) { }
		} x;

		bool valid;

		static optional<T, aborter> valid_dummy()
		{
			static_assert(std::is_same_v<T, void>, "cannot use valid_dummy() unless T = void");

			auto ret = optional<T, aborter>();
			ret.valid = true;

			return ret;
		}

		template <typename U, typename Ab, typename U1> friend optional<U1, Ab> krt::opt::some(U&&);
		template <typename U, typename Ab, typename U1> friend optional<U1, Ab> krt::opt::some(const U&);
		template <typename U, typename Ab, typename U1, typename... Args> friend optional<U1, Ab> krt::opt::some(Args&&...);

		template <typename X, typename Ab> friend struct optional;
	};


	namespace opt
	{
		template <typename U, typename Ab, typename U1 = std::remove_reference_t<U>>
		optional<U1, Ab> some(U&& x)
		{
			return optional<U1, Ab>(move(x));
		}

		template <typename U, typename Ab, typename U1 = std::remove_reference_t<U>>
		optional<U1, Ab> some(const U& x)
		{
			return optional<U1, Ab>(x);
		}

		template <typename U, typename Ab, typename U1 = std::remove_reference_t<U>, typename... Args>
		optional<U1, Ab> some(Args&&... xs)
		{
			return optional<U1, Ab>(forward<Args>(xs)...);
		}
	}
}
