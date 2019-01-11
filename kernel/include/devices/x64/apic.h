// apic.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"

namespace nx
{
	namespace acpi
	{
		struct MADTable;
	}

	namespace apic
	{
		void init(acpi::MADTable* madt);
	}
}