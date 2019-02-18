// errno.cpp
// Copyright (c) 2014 - 2016, zhiayang
// Licensed under the Apache License Version 2.0.

#include "../include/errno.h"
#include "../include/stdlib.h"

extern "C" {
	thread_local int _errno = 0;
}
