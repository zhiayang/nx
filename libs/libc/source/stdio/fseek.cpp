// fseek.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "../../include/stdio.h"
#include "../../include/unistd.h"

extern "C" int fseek(FILE* str, off_t offset, int origin)
{
	off_t ret = lseek((int) str->__fd, offset, origin);
	return ret == (off_t) -1 ? -1 : 0;
}

extern "C" off_t ftell(FILE* str)
{
	return tell((int) str->__fd);
}

extern "C" void rewind(FILE* str)
{
	fseek(str, 0, SEEK_SET);
}

extern "C" int fgetpos(FILE* str, fpos_t* pos)
{
	(void) str;
	*pos = ftell(str);

	return 0;
}

extern "C" int fsetpos(FILE* str, const fpos_t* pos)
{
	return fseek(str, *pos, SEEK_SET);
}





