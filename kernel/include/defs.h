// defs.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"
#include "sys/types.h"

#include "krt.h"

using addr_t = uintptr_t;

namespace nx
{
	constexpr size_t PAGE_SIZE = 0x1000;

	struct _allocator
	{
		static void* allocate(size_t sz, size_t align);
		static void deallocate(void* pt);

		template <typename T> static T* allocate(size_t n) { return (T*) allocate(n * sizeof(T), alignof(T)); }
	};

	struct _fixed_allocator
	{
		static void* allocate(size_t sz, size_t align);
		static void deallocate(void* pt);

		template <typename T> static T* allocate(size_t n) { return (T*) allocate(n * sizeof(T), alignof(T)); }
	};

	struct _aborter
	{
		static void abort(const char* fmt, ...);
		static void debuglog(const char* fmt, ...);
	};


	// re-export the krt types with our own stuff.
	using string = krt::string<_allocator, _aborter>;
	using string_view = krt::string_view<_allocator, _aborter>;

	template<typename T, typename Al = _allocator, typename Ab = _aborter>
	using list = krt::list<T, Al, Ab>;

	template<typename T, typename Al = _allocator, typename Ab = _aborter>
	using array = krt::array<T, Al, Ab>;

	template<typename T, typename Al = _allocator, typename Ab = _aborter>
	using stack = krt::stack<T, Al, Ab>;

	template<typename K, typename V, typename Al = _allocator, typename Ab = _aborter>
	using treemap = krt::treemap<K, V, Al, Ab>;

	[[noreturn]] void halt();

	[[noreturn]] void vabort(const char* fmt, va_list args);
	[[noreturn]] void abort(const char* fmt, ...);

	void vabort_nohalt(const char* fmt, va_list args);
	void abort_nohalt(const char* fmt, ...);

	[[noreturn]] void assert_fail(const char* file, size_t line, const char* thing);
	[[noreturn]] void assert_fail(const char* file, size_t line, const char* thing, const char* fmt, ...);

	struct PageExtent
	{
		PageExtent() : base(0), size(0) { }
		PageExtent(addr_t base, size_t size) : base(base), size(size) { }

		addr_t base;
		size_t size;
	};

	namespace scheduler
	{
		struct Thread;
		struct Process;
	}
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

#define VA_ARGS(...)            , ##__VA_ARGS__
#define kassert(x, fmt, ...)    ((x) ? ((void) 0) : ::nx::assert_fail(__FILE__, __LINE__, #x, fmt VA_ARGS(__VA_ARGS__)))


#define __unlikely(x)           __builtin_expect((x), 0)
#define __likely(x)             __builtin_expect((x), 1)





