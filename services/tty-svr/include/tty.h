// tty.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus

namespace ttysvr
{
	constexpr uint64_t MAGIC            = 0xA0E1'C600;

	constexpr uint16_t CMD_READ         = 1;
	constexpr uint16_t CMD_WRITE        = 2;
	constexpr uint16_t CMD_CREATE       = 3;
	constexpr uint16_t CMD_DESTROY      = 4;
	constexpr uint16_t CMD_GET_TTY      = 5;
	constexpr uint16_t CMD_SET_TTY      = 6;
	constexpr uint16_t CMD_SET_FLAG     = 7;
	constexpr uint16_t CMD_GET_FLAGS    = 8;

	constexpr uint16_t CMD_KB_INPUT     = 101;

	constexpr uint16_t FLAG_LINEBUFFER  = 0x1;
	constexpr uint16_t FLAG_INPUT_ECHO  = 0x2;

	struct payload_t
	{
		uint32_t magic;
		uint16_t tty;
		uint16_t cmd;

		size_t dataSize;
		uint8_t data[];

	} __attribute__ ((packed));
}

#else
	#error unsupported
#endif
