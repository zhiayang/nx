// kernel.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include "defs.h"

#include "fs.h"
#include "mm.h"
#include "ipc.h"
#include "synchro.h"

#include "devices/acpi.h"
#include "devices/ports.h"
#include "devices/serial.h"
#include "devices/console.h"

#include "cpu/cpu.h"
#include "cpu/scheduler.h"
#include "cpu/interrupts.h"
#include "cpu/exceptions.h"

#include "misc/util.h"
#include "misc/addrs.h"
#include "misc/syscall.h"
#include "misc/timekeeping.h"

#include "platform-specific.h"

#define NX_MIN_BOOTINFO_VERSION         1
#define NX_MAX_BOOTINFO_VERSION         3
#define NX_SYSCALL_INTERRUPT_VECTOR     0xF8

namespace nx
{
	namespace initrd
	{
		void init(BootInfo* bi);
	}


	// some misc stuff.
	void log(const char* sys, const char* fmt, ...);
	void warn(const char* sys, const char* fmt, ...);
	void error(const char* sys, const char* fmt, ...);


	void print(const char* fmt, ...);
	void println(const char* fmt, ...);
	int vprint(const char* fmt, va_list args);

	string sprint(const char* fmt, ...);

	static inline nx::string humanSizedBytes(size_t b, bool thou = false)
	{
		return krt::util::humanSizedBytes<_allocator, _aborter>(b, thou);
	}
}




















