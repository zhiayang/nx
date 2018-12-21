// fflush.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "../../include/stdio.h"
#include "../../include/orionx/syscall.h"

extern "C" int fflush(FILE* str)
{
	Library::SystemCall::Flush(str->__fd);
	return 0;
}
