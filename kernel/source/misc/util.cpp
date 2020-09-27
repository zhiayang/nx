// util.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace util
{
	nx::string plural(const nx::string& s, size_t count)
	{
		if(count == 1)  return sprint("1 {}", s.cstr());
		else            return sprint("{} {}s", count, s.cstr());
	}
}
}
















