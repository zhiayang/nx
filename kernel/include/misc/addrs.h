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

		constexpr uintptr_t USER_FRAMEBUFFER            = 0x0000'00FF'0000'0000;

		constexpr uintptr_t USER_KERNEL_STUBS           = 0x0000'7FFF'FFFF'0000;

		constexpr uintptr_t USER_ADDRSPACE_END          = 0x0000'7FFF'FFFF'F000;

		// ASLR some day??
		constexpr uintptr_t EFI_RUNTIME_SERVICES_BASE   = 0xFFFF'8000'0000'0000;

		// note: pml4[511] starts at 0xFFFF'FF80'0000'0000
		constexpr uintptr_t KERNEL_HEAP_BASE            = 0xFFFF'FF80'0000'0000;
		constexpr uintptr_t KERNEL_HEAP_END             = 0xFFFF'FFFF'0000'0000;

		constexpr uintptr_t KERNEL_FRAMEBUFFER          = 0xFFFF'FFFF'0000'0000;

		constexpr uintptr_t KERNEL_VMM_ADDRSPACE_BASE   = 0xFFFF'FFFF'8000'0000;
		constexpr uintptr_t KERNEL_VMM_ADDRSPACE_END    = 0xFFFF'FFFF'F3C0'0000;

		constexpr uintptr_t PMM_STACK_BASE              = 0xFFFF'FFFF'F3C0'0000;
		constexpr uintptr_t PMM_STACK_END               = 0xFFFF'FFFF'F3D0'0000;

		constexpr uintptr_t VMM_STACK0_BASE             = 0xFFFF'FFFF'F3D0'0000;
		constexpr uintptr_t VMM_STACK0_END              = 0xFFFF'FFFF'F3E0'0000;

		constexpr uintptr_t VMM_STACK1_BASE             = 0xFFFF'FFFF'F3E0'0000;
		constexpr uintptr_t VMM_STACK1_END              = 0xFFFF'FFFF'F3F0'0000;

		constexpr uintptr_t VMM_STACK2_BASE             = 0xFFFF'FFFF'F3F0'0000;
		constexpr uintptr_t VMM_STACK2_END              = 0xFFFF'FFFF'F400'0000;


		constexpr uintptr_t KERNEL_ENTRY                = 0xFFFF'FFFF'F800'0000;
	}
}
