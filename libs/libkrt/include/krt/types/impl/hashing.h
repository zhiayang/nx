// hashing.h
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once
#include <stddef.h>

#include <type_traits>

namespace krt
{
	// provide some default hashing implementations.

	template <typename T, typename = void>
	struct hash
	{
		size_t operator () (const T&) const = delete;
	};


	// integral types
	template <typename T>
	struct hash<T, std::enable_if_t<std::is_integral_v<T>>>
	{
		size_t operator () (const T& val) const
		{
			return (size_t) val;
		}
	};

	// pointer types.
	template <typename T>
	struct hash<T*>
	{
		size_t operator () (T* val) const
		{
			return (size_t) val;
		}
	};


	// classes with a hash() method
	template <typename T>
	struct hash<T, ::std::enable_if_t<::std::is_member_function_pointer_v<decltype(&T::hash)>>>
	{
		size_t operator () (const T& val) const
		{
			return val.hash();
		}
	};


	// builtin method to combine hashes
	template <typename T>
	size_t hash_combine(const T& x) { return hash<T>()(x); }


	template <typename T, typename... Args>
	size_t hash_combine(const T& x, Args&&... xs)
	{
		auto h = hash_combine(x);
		return h ^ (hash_combine(xs...) + 0x9E37'79B9'7F4A'7C15 + (h << 6) + (h >> 2));
	}
}
