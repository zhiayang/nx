// interrupts.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"

namespace nx
{
	namespace interrupts
	{
		void init();
		void enable();
		void disable();

		void maskIRQ(uint8_t num);
		void unmaskIRQ(uint8_t num);

		void setIDTEntry(uint8_t intr, addr_t fn, uint8_t codeSegment, uint8_t flags);
		void clearIDTEntry(uint8_t intr);
	}
}