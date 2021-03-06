// defs.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"
#include "sys/types.h"

#include "krt.h"

#ifdef ZPR_FREESTANDING
	#undef ZPR_FREESTANDING
#endif

#define ZPR_FREESTANDING 1
#include <zst/zpr.h>

using addr_t = uintptr_t;

namespace nx
{
	constexpr size_t PAGE_SIZE = 0x1000;

	[[noreturn]] void __internal_abort();
	void console_print(const char* s, size_t len);

	namespace interrupts
	{
		void enable();
		void disable();
	}

	namespace serial
	{
		void debugprintchar(char c);
		void debugprint(const char* c, size_t l);

		template <typename... Args>
		void debugprintf(const char* fmt, Args&&... args)
		{
			zpr::cprint(&serial::debugprint, fmt, static_cast<Args&&>(args)...);
		}
	}


	template <typename... Args>
	void print(const char* fmt, Args&&... args)
	{
		zpr::cprint(&console_print, fmt, static_cast<Args&&>(args)...);
	}

	template <typename... Args>
	void println(const char* fmt, Args&&... args)
	{
		zpr::cprintln(&console_print, fmt, static_cast<Args&&>(args)...);
	}

	template <typename... Args>
	[[noreturn]] void abort(const char* fmt, Args&&... args)
	{
		serial::debugprintf("\n\nkernel abort! error: ");
		serial::debugprintf(fmt, static_cast<Args&&>(args)...);

		__internal_abort();
	}

	template <typename... Args>
	[[noreturn]] void assert_fail(const char* file, size_t line, const char* thing, const char* fmt, Args&&... args)
	{
		interrupts::disable();

		println("failed assertion");
		println("invariant: {}", thing);
		print("reason:    ");
		print(fmt, static_cast<Args&&>(args)...);

		print("\nlocation:  {}:{}\n\n", file, line);

		__internal_abort();
	}



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
		template <typename... Args>
		static void abort(const char* fmt, Args&&... args)
		{
			abort(fmt, static_cast<Args&&>(args)...);
		}

		template <typename... Args>
		static void debuglog(const char* fmt, Args&&... args)
		{
			abort(fmt, static_cast<Args&&>(args)...);
		}
	};

	// re-export the krt types with our own stuff.
	using string = krt::string<_allocator, _aborter>;
	using string_view = krt::string_view<_allocator, _aborter>;

	template <typename T, typename Al = _allocator, typename Ab = _aborter>
	using list = krt::list<T, Al, Ab>;

	template <typename T, typename Al = _allocator, typename Ab = _aborter>
	using array = krt::array<T, Al, Ab>;

	template <typename T, typename Al = _allocator, typename Ab = _aborter>
	using stack = krt::stack<T, Al, Ab>;

	template <typename K, typename V, typename Al = _allocator, typename Ab = _aborter>
	using treemap = krt::treemap<K, V, Al, Ab>;

	template <typename K, typename V, typename H = krt::hash<K>, typename Al = _allocator, typename Ab = _aborter>
	using bucket_hashmap = krt::bucket_hashmap<K, V, Al, Ab, H>;

	template <typename K, typename H = krt::hash<K>, typename Al = _allocator, typename Ab = _aborter>
	using bucket_hashset = krt::bucket_hashset<K, Al, Ab, H>;

	template <typename T, typename Ab = _aborter>
	using optional = krt::optional<T, Ab>;

	// instead of doing alias templates (which can't be deduced in c++17), just make a wrapper function.
	namespace opt
	{
		template <typename T, typename T1 = std::remove_reference_t<T>>
		optional<T1> some(T&& x) { return krt::opt::some<T1, _aborter>(krt::forward<T>(x)); }

		template <typename T, typename T1 = std::remove_reference_t<T>, typename... Args>
		optional<T1> some(Args&&... xs) { return krt::opt::some<T1, _aborter>(krt::forward<Args>(xs)...); }

		constexpr auto none = krt::nullopt;
	}

	[[noreturn]] void halt();

	namespace scheduler
	{
		struct Thread;
		struct Process;
	}

	namespace vmm
	{
		// vmm.cpp stuff -- handles address space allocation + physical page allocation.
		enum class AddressSpaceType
		{
			Kernel      = 0,
			KernelHeap  = 1,
			User        = 2
		};
		constexpr size_t NumAddressSpaces = 3;

		constexpr uint64_t PAGE_PRESENT         = 0x1;
		constexpr uint64_t PAGE_WRITE           = 0x2;
		constexpr uint64_t PAGE_USER            = 0x4;
		constexpr uint64_t PAGE_COPY_ON_WRITE   = 0x200;
		constexpr uint64_t PAGE_NX              = 0x8000'0000'0000'0000;
		constexpr uint64_t ALIGN_BITS           = 12;
	}




	template <typename... Args>
	nx::string sprint(const char* fmt, Args&&... args)
	{
		nx::string ret;
		zpr::cprint([&ret](const char* s, size_t len) {

			ret += nx::string_view(s, len);

		}, fmt, static_cast<Args&&>(args)...);

		return ret;
	}
}

// wow fucking c++ is so poorly designed
namespace std
{
	enum class align_val_t : size_t { };
	// using max_align_t = long double;
}

[[nodiscard]] void* operator new    (size_t count);
[[nodiscard]] void* operator new[]  (size_t count);
[[nodiscard]] void* operator new    (size_t count, std::align_val_t al);
[[nodiscard]] void* operator new[]  (size_t count, std::align_val_t al);
[[nodiscard]] void* operator new    (size_t count, void* ptr);
[[nodiscard]] void* operator new[]  (size_t count, void* ptr);

void operator delete    (void* ptr) noexcept;
void operator delete[]  (void* ptr) noexcept;
void operator delete    (void* ptr, size_t al);
void operator delete[]  (void* ptr, size_t al);



#ifdef assert
#undef assert
#endif

#define assert(x)               ((x) ? ((void) 0) : ::nx::assert_fail(__FILE__, __LINE__, #x, ""))

#define __unlikely(x)           __builtin_expect((x), 0)
#define __likely(x)             __builtin_expect((x), 1)







// strong typedefs for addresses.
namespace nx
{
	constexpr addr_t PAGE_ALIGN(addr_t addr)
	{
		// check if we have the 62-nd bit set, to determine if we actually had the uppermost bit set.
		// obviously when the virtual address space increases to 62-bits or more from 48-bits, then
		// this ain't gonna work.
		if(addr & 0x4000'0000'0000'0000)    return addr & 0xFFFF'FFFF'FFFF'F000;
		else                                return addr & 0x7FFF'FFFF'FFFF'F000;
	}

	constexpr bool isPageAligned(addr_t addr)
	{
		return addr == (addr & ~((addr_t) 0xFFF));
	}


	struct __TagVirt { };
	struct __TagPhys { };

	struct ofsPages
	{
		explicit ofsPages(size_t pages) : bytes(pages * PAGE_SIZE) { }

		ofsPages(const ofsPages&) = delete;
		ofsPages& operator = (ofsPages&&) = delete;
		ofsPages& operator = (const ofsPages&) = delete;

		template <typename T> friend struct __AddressImpl;

	private:
		size_t bytes;
	};


	template <typename Tag>
	struct __AddressImpl
	{
	private:
		using Self = __AddressImpl;
		addr_t m_addr;

	public:
		constexpr __AddressImpl() : m_addr(0) { }
		constexpr explicit __AddressImpl(addr_t x) : m_addr(x) { }
		constexpr explicit __AddressImpl(void* x) : m_addr((addr_t) x) { }

		~__AddressImpl() = default;
		constexpr __AddressImpl(Self&&) = default;
		constexpr __AddressImpl(const Self&) = default;
		constexpr Self& operator = (Self&&) = default;
		constexpr Self& operator = (const Self&) = default;

		constexpr bool operator == (Self oth) const { return this->m_addr == oth.m_addr; }
		constexpr bool operator != (Self oth) const { return this->m_addr != oth.m_addr; }
		constexpr bool operator >= (Self oth) const { return this->m_addr >= oth.m_addr; }
		constexpr bool operator <= (Self oth) const { return this->m_addr <= oth.m_addr; }
		constexpr bool operator >  (Self oth) const { return this->m_addr > oth.m_addr; }
		constexpr bool operator <  (Self oth) const { return this->m_addr < oth.m_addr; }

		constexpr inline bool nonZero() const { return !this->isZero(); }
		constexpr inline bool isZero() const { return this->m_addr == 0; }

		constexpr inline bool isPageAligned() const { return nx::isPageAligned(this->m_addr); }
		constexpr Self pageAligned() const { return Self(PAGE_ALIGN(this->m_addr)); }

		constexpr inline addr_t addr() const { return this->m_addr; }
		constexpr inline void* ptr() const { return (void*) this->m_addr; }

		constexpr inline bool isAlignedTo(size_t alignment) { return (this->m_addr & (alignment - 1)) == 0; }

		constexpr size_t operator - (const Self& oth) const { return this->m_addr - oth.m_addr; }

		constexpr Self operator + (size_t ofs) const { return Self(this->m_addr + ofs); }
		constexpr Self operator - (size_t ofs) const { return Self(this->m_addr - ofs); }
		constexpr Self& operator += (size_t ofs) { this->m_addr += ofs; return *this; }
		constexpr Self& operator -= (size_t ofs) { this->m_addr -= ofs; return *this; }

		constexpr Self operator + (ofsPages&& ofs) const { return Self(this->m_addr + ofs.bytes); }
		constexpr Self operator - (ofsPages&& ofs) const { return Self(this->m_addr - ofs.bytes); }
		constexpr Self& operator += (ofsPages&& ofs) { this->m_addr += ofs.bytes; return *this; }
		constexpr Self& operator -= (ofsPages&& ofs) { this->m_addr -= ofs.bytes; return *this; }



		inline void clear() { this->m_addr = 0; }

		constexpr inline size_t hash() const
		{
			return (size_t) this->m_addr;
		}

		template <typename E = Tag>
		constexpr std::enable_if_t<std::is_same_v<E, __TagVirt>, __AddressImpl<__TagPhys>>
		physIdentity() const
		{
			return __AddressImpl<__TagPhys>(this->m_addr);
		}

		static Self zero() { return Self(nullptr); }
	};

	using PhysAddr = __AddressImpl<__TagPhys>;
	using VirtAddr = __AddressImpl<__TagVirt>;


	template <typename Kind>
	struct PageExtent
	{
		PageExtent() : base(0), size(0) { }
		PageExtent(Kind base, size_t size) : base(base), size(size) { }

		Kind base;
		size_t size;
	};
}



