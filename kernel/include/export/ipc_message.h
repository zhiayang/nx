// ipc_message.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include <stdint.h>

namespace nx {
namespace ipc
{
	// should be respectable enough. anything more pls use shared mem
	constexpr size_t MAX_MESSAGE_SIZE   = 0x4000;
}
}
