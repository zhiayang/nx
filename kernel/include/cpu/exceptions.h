// exceptions.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"

namespace nx
{
	struct BootInfo;
	namespace exceptions
	{
		void init(BootInfo* bi);
	}
}