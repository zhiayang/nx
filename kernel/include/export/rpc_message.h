// rpc_message.h
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once
#include <stddef.h>
#include <stdint.h>

namespace nx {
namespace rpc
{
	constexpr uint32_t RPC_OK                   = 0;    // no error
	constexpr uint32_t RPC_ERR_NO_CONNECTION    = 1;    // the connection does not exist
	constexpr uint32_t RPC_ERR_NO_FUNCTION      = 2;    // the function does not exist
	constexpr uint32_t RPC_ERR_INVALID_ARGS     = 3;    // the arguments were invalid
	constexpr uint32_t RPC_ERR_WRONG_CONNECTION = 4;    // the connection does not belong to the calling proc

	struct message_t
	{
		pid_t counterpart;
		uint64_t function;
		uint32_t sequence;
		uint32_t status;

		union {
			uint64_t args[12];
			uint8_t bytes[12 * sizeof(uint64_t)];
		};
	};
}
}
