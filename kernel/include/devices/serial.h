// serial.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"

namespace nx
{
	namespace serial
	{
		void init();
		void initReceive(void (*callback)(uint8_t));

		void debugprintchar(char c);
		void debugprint(const char* c, size_t l);
		void debugprint(const char* s);
	}
}
