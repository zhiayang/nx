// fmisc.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "../../include/stdio.h"

#define FLAG_EOF	0x80
#define FLAG_ERR	0x40

extern "C" void clearerr(FILE* str)
{
	str->indicatorflags &= ~FLAG_ERR;
}

extern "C" int ferror(FILE* str)
{
	return str->indicatorflags & FLAG_ERR;
}

extern "C" int feof(FILE* str)
{
	return str->indicatorflags & FLAG_EOF;
}
