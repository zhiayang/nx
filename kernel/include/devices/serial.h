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
		void debugprint(char c);
		void debugprint(char* c, size_t l);
		void debugprint(const char* s);

		void debugprintf(const char* fmt, ...);
	}
}