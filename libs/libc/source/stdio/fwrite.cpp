// fwrite.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "../../include/unistd.h"

extern "C" size_t fwrite(const void* ptr, size_t size, size_t count, FILE* stream)
{
	// stdout buffering will be handled by the kernel.
	return write((int) stream->__fd, ptr, count * size);
}
