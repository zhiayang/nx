// params.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "defs.h"

namespace nx
{
	struct BootInfo;
	namespace params
	{
		void init(BootInfo* bi);

		bool haveOption(const nx::string& name);
		nx::string getOptionValue(const nx::string& name);
	}
}










