// fflush.cpp
// Copyright (c) 2014 - 2016, zhiayang
// Licensed under the Apache License Version 2.0.

#include "../../include/stdio.h"

extern "C" int fflush(FILE* str)
{
	// Library::SystemCall::Flush(str->__fd);
	return 0;
}
