// fgetc.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "../../include/stdio.h"
#include "../../include/unistd.h"
#include "../../include/assert.h"


extern "C" int fgetc(FILE* str)
{
	int f = 0;
	fread(&f, sizeof(char), 1, str);
	return f;
}

extern "C" int getc(FILE* str)
{
	return fgetc(str);
}

extern "C" int getchar()
{
	return fgetc(stdin);
}
