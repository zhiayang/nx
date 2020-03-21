// hashing.h
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once
#include <stddef.h>

#include <type_traits>

namespace krt
{
	template <typename T>
	struct hash
	{
		static_assert("hash function not implemented!");
		size_t operator () (const T& val) const = delete;
	};



	template <> struct hash<char>               { size_t operator () (char x)       { return x; } };
	template <> struct hash<short>              { size_t operator () (short x)      { return x; } };
	template <> struct hash<int>                { size_t operator () (int x)        { return x; } };
	template <> struct hash<long>               { size_t operator () (long x)       { return x; } };
	template <> struct hash<long long>          { size_t operator () (long long x)  { return x; } };

	template <> struct hash<unsigned char>      { size_t operator () (unsigned char x)      { return x; } };
	template <> struct hash<unsigned short>     { size_t operator () (unsigned short x)     { return x; } };
	template <> struct hash<unsigned int>       { size_t operator () (unsigned int x)       { return x; } };
	template <> struct hash<unsigned long>      { size_t operator () (unsigned long x)      { return x; } };
	template <> struct hash<unsigned long long> { size_t operator () (unsigned long long x) { return x; } };


	template <typename T>
	struct hash<T*>
	{
		size_t operator () (T* val) { return (size_t) val; }
	};
}
