// fread.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "../../include/unistd.h"
#include "../../include/assert.h"
#include "../../include/stdlib.h"
#include "../../include/string.h"

extern "C" size_t fread(void* ptr, size_t esize, size_t length, FILE* stream)
{
	return read((int) stream->__fd, ptr, esize * length);
}










