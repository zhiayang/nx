// meta.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

namespace krt
{
	// omg why the fuck are we doing this?!
	template <typename T, T v>
	struct integral_constant
	{
		static constexpr T value = v;
		typedef T value_type;
		typedef integral_constant type; // using injected-class-name

		constexpr operator value_type() const { return value; }
		constexpr value_type operator()() const { return value; }
	};

	using true_type = integral_constant<bool, true>;
	using false_type = integral_constant<bool, false>;

	template <typename T, typename U>
	struct is_same : false_type {};

	template <typename T>
	struct is_same<T, T> : true_type {};

	template <typename T> struct remove_reference      { typedef T type; };
	template <typename T> struct remove_reference<T&>  { typedef T type; };
	template <typename T> struct remove_reference<T&&> { typedef T type; };

	template <typename T>
	typename remove_reference<T>::type&& move(T&& arg)
	{
		return static_cast<typename remove_reference<T>::type&&>(arg);
	}

	template <typename T>
	void swap(T& t1, T& t2)
	{
		T temp = krt::move(t1);
		t1 = krt::move(t2);
		t2 = krt::move(temp);
	}

	template <typename T>
	struct identity { typedef T type; };

	template <typename T> T&& forward(typename remove_reference<T>::type& t)  { return static_cast<typename identity<T>::type&&>(t); }
	template <typename T> T&& forward(typename remove_reference<T>::type&& t) { return static_cast<typename identity<T>::type&&>(t); }

	template<class T, class U = T>
	T exchange(T& obj, U&& new_value)
	{
		T old_value = krt::move(obj);
		obj = krt::forward<U>(new_value);
		return old_value;
	}
}


















