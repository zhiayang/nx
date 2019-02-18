// tty.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus

namespace ttysvr
{
	constexpr uint64_t MAGIC        = 0xA0E1'C600;

	struct payload_t
	{
		uint32_t magic;
		uint32_t tty;

		size_t dataSize;
		uint8_t data[];

	} __attribute__ ((packed));
}

#else
	#error unsupported
#endif
