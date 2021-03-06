// meta.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include <type_traits>

#include "types/impl/hashing.h"

namespace krt
{
	// template <typename T> struct remove_reference      { typedef T type; };
	// template <typename T> struct remove_reference<T&>  { typedef T type; };
	// template <typename T> struct remove_reference<T&&> { typedef T type; };

	template <typename T>
	typename std::remove_reference<T>::type&& move(T&& arg)
	{
		return static_cast<typename std::remove_reference<T>::type&&>(arg);
	}

	// // omg why the fuck are we doing this?!
	// template <typename T, T v>
	// struct integral_constant
	// {
	// 	static constexpr T value = v;
	// 	typedef T value_type;
	// 	typedef integral_constant type; // using injected-class-name

	// 	constexpr operator value_type() const { return value; }
	// 	constexpr value_type operator()() const { return value; }
	// };

	// using true_type = integral_constant<bool, true>;
	// using false_type = integral_constant<bool, false>;


	template <typename T, typename U>
	struct pair
	{
		pair() { }
		pair(const T& f, const U& s) : first(f), second(s) { }

		T first;
		U second;
	};

	template <typename T, typename U, typename V>
	struct triple
	{
		triple() { }
		triple(const T& f, const U& s, const V& t) : first(f), second(s), third(t) { }

		T first;
		U second;
		V third;
	};

	template <typename T = void>
	struct less
	{
		constexpr bool operator () (const T& lhs, const T& rhs) const
		{
			return compare(lhs, rhs);
		}

		static constexpr bool compare(const T& lhs, const T& rhs)
		{
			return lhs < rhs;
		}
	};

	template <typename T = void>
	struct greater
	{
		constexpr bool operator () (const T& lhs, const T& rhs) const
		{
			return compare(lhs, rhs);
		}

		static constexpr bool compare(const T& lhs, const T& rhs)
		{
			return lhs > rhs;
		}
	};




	template <typename T>
	bool match(const T& first)
	{
		return true;
	}

	template <typename T, typename U>
	bool match(const T& first, const U& second)
	{
		return (first == second);
	}

	template <typename T, typename U, typename... Args>
	bool match(const T& first, const U& second, const Args&... comps)
	{
		return (first == second) || match(first, comps...);
	}



	// template <typename T, typename U>
	// struct is_same : false_type {};

	// template <typename T>
	// struct is_same<T, T> : true_type {};

	// template <typename T>
	// struct is_trivially_copyable
	// {
	// 	static constexpr bool value = __has_trivial_copy(T);
	// };

	template <typename T>
	void swap(T& t1, T& t2)
	{
		T temp = krt::move(t1);
		t1 = krt::move(t2);
		t2 = krt::move(temp);
	}

	template <typename T>
	struct identity { typedef T type; };

	template <typename T>
	T&& forward(typename std::remove_reference<T>::type& t)
	{
		return static_cast<typename identity<T>::type&&>(t);
	}

	template <typename T>
	T&& forward(typename std::remove_reference<T>::type&& t)
	{
		return static_cast<typename identity<T>::type&&>(t);
	}

	template<class T, class U = T>
	T exchange(T& obj, U&& new_value)
	{
		T old_value = krt::move(obj);
		obj = krt::forward<U>(new_value);
		return old_value;
	}


	template <typename T>
	T max(const T& a, const T& b)
	{
		return a > b ? a : b;
	}

	template <typename T>
	T min(const T& a, const T& b)
	{
		return a < b ? a : b;
	}
}


















