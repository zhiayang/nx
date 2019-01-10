// console.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"

namespace nx
{
	namespace console
	{
		void init(int x, int y, int scany);
		void init_stage2();

		void clear();
		void putchar(char c);

		namespace fallback
		{
			void init(int x, int y, int scany);
			void putchar(char c);

			void background(uint32_t bg);
			void foreground(uint32_t fg);
		}
	}
}
