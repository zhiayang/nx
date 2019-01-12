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
		constexpr uintptr_t USER_ADDRSPACE_BASE         = 0x0040'0000;
		constexpr uintptr_t USER_ADDRSPACE_END          = 0xFFF8'0000;

		// ASLR some day??

		constexpr uintptr_t KERNEL_HEAP_BASE            = 0xFFFFFFFF'0000'0000;
		constexpr uintptr_t KERNEL_HEAP_END             = 0xFFFFFFFF'8000'0000;

		constexpr uintptr_t KERNEL_ENTRY                = 0xFFFFFFFF'8000'0000;
		constexpr uintptr_t KERNEL_CODE_END             = 0xFFFFFFFF'9000'0000;

		constexpr uintptr_t KERNEL_VMM_ADDRSPACE_BASE   = 0xFFFFFFFF'9000'0000;
		constexpr uintptr_t KERNEL_VMM_ADDRSPACE_END    = 0xFFFFFFFF'D000'0000;

		constexpr uintptr_t VMM_STACK0_BASE             = 0xFFFFFFFF'DA00'0000;
		constexpr uintptr_t VMM_STACK0_END              = 0xFFFFFFFF'DB00'0000;

		constexpr uintptr_t VMM_STACK1_BASE             = 0xFFFFFFFF'DB00'0000;
		constexpr uintptr_t VMM_STACK1_END              = 0xFFFFFFFF'DC00'0000;

		constexpr uintptr_t VMM_STACK2_BASE             = 0xFFFFFFFF'DC00'0000;
		constexpr uintptr_t VMM_STACK2_END              = 0xFFFFFFFF'DD00'0000;

		constexpr uintptr_t PMM_STACK_BASE              = 0xFFFFFFFF'DD00'0000;
		constexpr uintptr_t PMM_STACK_END               = 0xFFFFFFFF'DE00'0000;

		constexpr uintptr_t EFI_RUNTIME_SERVICES_BASE   = 0xFFFFFFFF'DE00'0000;
		constexpr uintptr_t KERNEL_FRAMEBUFFER          = 0xFFFFFFFF'E000'0000;
	}
}