// defs.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"

#include "krt.h"

using addr_t = uintptr_t;

namespace nx
{
	struct _allocator
	{
		static void* allocate(size_t sz, size_t align);
		static void deallocate(void* pt);
	};

	struct _aborter
	{
		static void abort(const char* fmt, ...);
		static void debuglog(const char* fmt, ...);
	};


	// re-export the krt types with our own stuff.
	using string = krt::string<_allocator, _aborter>;

	template<typename T> using array = krt::array<T, _allocator, _aborter>;
	template<typename T> using stack = krt::stack<T, _allocator, _aborter>;
	template<typename K, typename V> using treemap = krt::treemap<K, V, _allocator, _aborter>;

	[[noreturn]] void halt();

	[[noreturn]] void vabort(const char* fmt, va_list args);
	[[noreturn]] void abort(const char* fmt, ...);

	void vabort_nohalt(const char* fmt, va_list args);
	void abort_nohalt(const char* fmt, ...);

	[[noreturn]] void assert_fail(const char* file, size_t line, const char* thing);
	[[noreturn]] void assert_fail(const char* file, size_t line, const char* thing, const char* fmt, ...);
}

// wow fucking c++ is so poorly designed
namespace std
{
	#ifdef FUCKIN_WSL_FIX_YOUR_SHIT
	using size_t = unsigned long long;
	#else
	using size_t = ::size_t;
	#endif

	enum class align_val_t : size_t { };

	using max_align_t = long double;
}

[[nodiscard]] void* operator new    (std::size_t count);
[[nodiscard]] void* operator new[]  (std::size_t count);
[[nodiscard]] void* operator new    (std::size_t count, std::align_val_t al);
[[nodiscard]] void* operator new[]  (std::size_t count, std::align_val_t al);
[[nodiscard]] void* operator new    (std::size_t count, void* ptr);
[[nodiscard]] void* operator new[]  (std::size_t count, void* ptr);

void operator delete    (void* ptr) noexcept;
void operator delete[]  (void* ptr) noexcept;
void operator delete    (void* ptr, size_t al);
void operator delete[]  (void* ptr, size_t al);



#ifdef assert
#undef assert
#endif

#define assert(x)               ((x) ? ((void) 0) : ::nx::assert_fail(__FILE__, __LINE__, #x))
#define kassert(x, fmt, ...)    ((x) ? ((void) 0) : ::nx::assert_fail(__FILE__, __LINE__, #x, fmt, __VA_ARGS__))








