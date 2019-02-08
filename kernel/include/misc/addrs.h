// addrs.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"

namespace nx
{
	namespace addrs
	{
		constexpr uintptr_t USER_ADDRSPACE_BASE         = 0x0000'0000'0040'0000;
		constexpr uintptr_t USER_ADDRSPACE_END          = 0x0000'7FFF'FFFF'F000;

		// ASLR some day??
		constexpr uintptr_t KERNEL_VMM_ADDRSPACE_BASE   = 0xFFFF'8000'0000'0000;
		constexpr uintptr_t KERNEL_VMM_ADDRSPACE_END    = 0xFFFF'FFF0'0000'0000;

		constexpr uintptr_t KERNEL_HEAP_BASE            = 0xFFFF'FFF0'0000'0000;
		constexpr uintptr_t KERNEL_HEAP_END             = 0xFFFF'FFFF'0000'0000;

		constexpr uintptr_t KERNEL_FRAMEBUFFER          = 0xFFFF'FFFF'0000'0000;


		constexpr uintptr_t PMM_STACK_BASE              = 0xFFFF'FFFF'F000'0000;
		constexpr uintptr_t PMM_STACK_END               = 0xFFFF'FFFF'F010'0000;

		constexpr uintptr_t VMM_STACK0_BASE             = 0xFFFF'FFFF'F010'0000;
		constexpr uintptr_t VMM_STACK0_END              = 0xFFFF'FFFF'F020'0000;

		constexpr uintptr_t VMM_STACK1_BASE             = 0xFFFF'FFFF'F020'0000;
		constexpr uintptr_t VMM_STACK1_END              = 0xFFFF'FFFF'F030'0000;

		constexpr uintptr_t VMM_STACK2_BASE             = 0xFFFF'FFFF'F030'0000;
		constexpr uintptr_t VMM_STACK2_END              = 0xFFFF'FFFF'F040'0000;



		constexpr uintptr_t EFI_RUNTIME_SERVICES_BASE   = 0xFFFF'FFFF'F400'0000;

		constexpr uintptr_t KERNEL_ENTRY                = 0xFFFF'FFFF'F800'0000;
	}
}