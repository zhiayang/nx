// vfs.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <stddef.h>

#include <ipc.h>

namespace vfs
{
	constexpr int MNT_RDONLY = 1;

	int mount(const char* mountpoint, nx::ipc::selector_t* sel_driver,
		nx::ipc::selector_t* sel_device, int flags);
}
