// ports.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"

namespace nx
{
	namespace port
	{
		uint8_t read1b(uint16_t port);
		uint16_t read2b(uint16_t port);
		uint32_t read4b(uint16_t port);

		void write1b(uint16_t port, uint8_t value);
		void write2b(uint16_t port, uint16_t value);
		void write4b(uint16_t port, uint32_t value);
	}
}
