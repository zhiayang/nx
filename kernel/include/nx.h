// kernel.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include "defs.h"

#include "mm.h"
#include "bootinfo.h"

#include "devices/ports.h"
#include "devices/serial.h"

#include "krt.h"

namespace nx
{
	constexpr size_t PAGE_SIZE                      = 0x1000;
	namespace addrs
	{
		constexpr addr_t USER_ADDRSPACE_BASE        = 0x0400'0000;
		constexpr addr_t USER_ADDRSPACE_END         = 0xFFFF'F000;


		// ASLR some day??
		constexpr addr_t KERNEL_HEAP_BASE           = 0xFFFFFFFF'0000'0000;
		constexpr addr_t KERNEL_HEAP_END            = 0xFFFFFFFF'8000'0000;

		constexpr addr_t KERNEL_ENTRY               = 0xFFFFFFFF'8000'0000;
		constexpr addr_t KERNEL_CODE_END            = 0xFFFFFFFF'9000'0000;

		constexpr addr_t KERNEL_VMM_ADDRSPACE_BASE  = 0xFFFFFFFF'9000'0000;
		constexpr addr_t KERNEL_VMM_ADDRSPACE_END   = 0xFFFFFFFF'D000'0000;

		constexpr addr_t VMM_STACK0_BASE            = 0xFFFFFFFF'DA00'0000;
		constexpr addr_t VMM_STACK0_END             = 0xFFFFFFFF'DB00'0000;

		constexpr addr_t VMM_STACK1_BASE            = 0xFFFFFFFF'DB00'0000;
		constexpr addr_t VMM_STACK1_END             = 0xFFFFFFFF'DC00'0000;

		constexpr addr_t VMM_STACK2_BASE            = 0xFFFFFFFF'DC00'0000;
		constexpr addr_t VMM_STACK2_END             = 0xFFFFFFFF'DD00'0000;

		constexpr addr_t PMM_STACK_BASE             = 0xFFFFFFFF'DD00'0000;
		constexpr addr_t PMM_STACK_END              = 0xFFFFFFFF'DE00'0000;

		constexpr addr_t EFI_RUNTIME_SERVICES_BASE  = 0xFFFFFFFF'DE00'0000;
		constexpr addr_t KERNEL_FRAMEBUFFER         = 0xFFFFFFFF'E000'0000;
	}

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



	void kernel_main(BootInfo* bootinfo);

	// some misc stuff.
	void print(const char* fmt, ...);
	void println(const char* fmt, ...);

	string sprint(const char* fmt, ...);

	[[noreturn]] void vabort(const char* fmt, va_list args);
	[[noreturn]] void abort(const char* fmt, ...);

	#ifdef assert
	#undef assert
	#endif

	[[noreturn]] void assert_fail(const char* file, size_t line, const char* thing);

}

#define assert(x)   ((x) ? ((void) 0) : ::nx::assert_fail(__FILE__, __LINE__, #x))






















