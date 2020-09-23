// rpc_message.h
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

namespace nx {
namespace rpc
{
	constexpr uint32_t RPC_OK                   = 0;    // no error
	constexpr uint32_t RPC_ERR_NO_CONNECTION    = 1;    // the connection does not exist
	constexpr uint32_t RPC_ERR_NO_FUNCTION      = 2;    // the function does not exist
	constexpr uint32_t RPC_ERR_INVALID_ARGS     = 3;    // the arguments were invalid
	constexpr uint32_t RPC_ERR_WRONG_CONNECTION = 4;    // the connection does not belong to the calling proc
	constexpr uint32_t RPC_ERR_CALL             = 5;    // the server returned an error (check the arguments)

	struct message_t
	{
		// filled in by the kernel, for the opposing endpoint to use
		pid_t counterpart;

		uint64_t function;

		// this is filled in by the kernel
		uint32_t sequence;

		/*
			this is filled in by the kernel for calls, but is should be filled in
			by the server for return values. (errors should be indicated with RPC_ERR_CALL)
			and the error-code (or other information) should be returned in the params in an
			application-defined manner.

			the convention is that an error-code should be the first 64-bit value, ie.
			`args[0]`; any extra information is optionally appended.
		*/
		uint32_t status;

		// this is the data -- both for call parameters and the return result.
		union {
			uint64_t args[12];
			uint8_t bytes[12 * sizeof(uint64_t)];
		};

		static constexpr size_t MAX_SIZE = 12 * sizeof(uint64_t);
	};
}
}
