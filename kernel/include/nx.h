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
		constexpr addr_t KERNEL_ENTRY               = 0xFFFF'FFFF'8000'0000;
		constexpr addr_t PMM_STACK_BASE             = 0xFFFF'FFFF'C000'0000;
		constexpr addr_t EFI_RUNTIME_SERVICES_BASE  = 0xFFFF'FFFF'CE00'0000;
		constexpr addr_t KERNEL_FRAMEBUFFER         = 0xFFFF'FFFF'D000'0000;
	}

	struct _allocator
	{
		static void* allocate(size_t sz);
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

	[[noreturn]] void abort(const char* fmt, ...);
}













