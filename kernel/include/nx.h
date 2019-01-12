// kernel.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include "defs.h"

#include "fs.h"
#include "mm.h"
#include "bootinfo.h"

#include "devices/acpi.h"
#include "devices/ports.h"
#include "devices/serial.h"
#include "devices/console.h"

#include "cpu/scheduler.h"
#include "cpu/interrupts.h"
#include "cpu/exceptions.h"

#include "misc/util.h"
#include "misc/addrs.h"

#include "platform-specific.h"



namespace nx
{
	constexpr size_t PAGE_SIZE                      = 0x1000;


	void kernel_main(BootInfo* bootinfo);



	namespace initrd
	{
		void init(BootInfo* bi);
	}







	// some misc stuff.

	void print(const char* fmt, ...);
	void println(const char* fmt, ...);
	int vprint(const char* fmt, va_list args);

	string sprint(const char* fmt, ...);

	static inline nx::string humanSizedBytes(size_t b, bool thou = false)
	{
		return krt::util::humanSizedBytes<_allocator, _aborter>(b, thou);
	}
}




















