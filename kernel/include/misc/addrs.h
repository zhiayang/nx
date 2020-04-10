// addrs.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"

#include "../defs.h"

namespace nx
{
	namespace addrs
	{
		constexpr auto USER_ADDRSPACE_BASE          = VirtAddr(0x0000'0000'0040'0000);

		constexpr auto USER_FRAMEBUFFER             = VirtAddr(0x0000'00FF'0000'0000);

		constexpr auto USER_ADDRSPACE_END           = VirtAddr(0x0000'7FFF'FFFF'8000);

		constexpr auto USER_KERNEL_STUBS            = VirtAddr(0x0000'7FFF'FFFF'8000);

		// ASLR some day??
		constexpr auto EFI_RUNTIME_SERVICES_BASE    = VirtAddr(0xFFFF'8000'0000'0000);

		// pml4[509] = 0xFFFF'FE80'0000'0000
		// pml4[510] = 0xFFFF'FF00'0000'0000

		// note: pml4[511] starts at 0xFFFF'FF80'0000'0000
		constexpr auto KERNEL_HEAP_BASE             = VirtAddr(0xFFFF'FF80'0000'0000);
		constexpr auto KERNEL_HEAP_END              = VirtAddr(0xFFFF'FFFF'0000'0000);

		constexpr auto KERNEL_FRAMEBUFFER           = VirtAddr(0xFFFF'FFFF'0000'0000);

		constexpr auto KERNEL_VMM_ADDRSPACE_BASE    = VirtAddr(0xFFFF'FFFF'8000'0000);
		constexpr auto KERNEL_VMM_ADDRSPACE_END     = VirtAddr(0xFFFF'FFFF'F3C0'0000);

		constexpr auto PMM_STACK_BASE               = VirtAddr(0xFFFF'FFFF'F3C0'0000);
		constexpr auto PMM_STACK_END                = VirtAddr(0xFFFF'FFFF'F3D0'0000);

		constexpr auto VMM_STACK0_BASE              = VirtAddr(0xFFFF'FFFF'F3D0'0000);
		constexpr auto VMM_STACK0_END               = VirtAddr(0xFFFF'FFFF'F3E0'0000);

		constexpr auto VMM_STACK1_BASE              = VirtAddr(0xFFFF'FFFF'F3E0'0000);
		constexpr auto VMM_STACK1_END               = VirtAddr(0xFFFF'FFFF'F3F0'0000);

		constexpr auto VMM_STACK2_BASE              = VirtAddr(0xFFFF'FFFF'F3F0'0000);
		constexpr auto VMM_STACK2_END               = VirtAddr(0xFFFF'FFFF'F400'0000);


		constexpr auto KERNEL_ENTRY                 = VirtAddr(0xFFFF'FFFF'F800'0000);
	}
}
