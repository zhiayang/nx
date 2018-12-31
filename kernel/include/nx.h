// kernel.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"

#include "mm.h"
#include "bootinfo.h"

#include "devices/ports.h"
#include "devices/serial.h"

#include "krt.h"

namespace nx
{
	namespace consts
	{
		constexpr uintptr_t KERNEL_ENTRY                = 0xFFFFFFFF'80000000;
		constexpr uintptr_t EFI_RUNTIME_SERVICES_BASE   = 0xFFFFFFFF'C0000000;
		constexpr uintptr_t KERNEL_FRAMEBUFFER_ADDRESS  = 0xFFFFFFFF'D0000000;
	}

	struct allocator
	{
		static void* allocate(size_t sz);
		static void deallocate(void* pt);
	};

	struct aborter
	{
		static void abort(const char* fmt, ...);
		static void debuglog(const char* fmt, ...);
	};


	// re-export the krt types with our own stuff.
	using string = krt::string<allocator, aborter>;

	template<typename T> using array = krt::array<T, allocator, aborter>;
	template<typename T> using stack = krt::stack<T, allocator, aborter>;




	void kernel_main(BootInfo* bootinfo);

	// some misc stuff.
	void print(const char* fmt, ...);
	void println(const char* fmt, ...);

	string sprint(const char* fmt, ...);
}













