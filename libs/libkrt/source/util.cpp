// util.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "krt.h"

namespace krt {
namespace util
{
	void memfill1b(uint8_t* ptr, uint8_t val, size_t count)
	{
		for(size_t i = 0; i < count; i++)
			ptr[i] = val;
	}

	void memfill2b(uint16_t* ptr, uint16_t val, size_t count)
	{
		for(size_t i = 0; i < count; i++)
			ptr[i] = val;
	}

	void memfill4b(uint32_t* ptr, uint32_t val, size_t count)
	{
		for(size_t i = 0; i < count; i++)
			ptr[i] = val;
	}

	void memfill8b(uint64_t* ptr, uint64_t val, size_t count)
	{
		for(size_t i = 0; i < count; i++)
			ptr[i] = val;
	}
}
}



















