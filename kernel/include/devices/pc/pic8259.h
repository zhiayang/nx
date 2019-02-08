// pic.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "defs.h"

namespace nx
{
	namespace device
	{
		namespace pic8259
		{
			void init();
			void disable();

			void sendEOI(uint8_t num);

			void maskIRQ(uint8_t num);
			void unmaskIRQ(uint8_t num);
		}
	}
}