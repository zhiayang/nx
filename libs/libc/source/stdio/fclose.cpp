// fclose.cpp
// Copyright (c) 2014 - 2016, zhiayang
// Licensed under the Apache License Version 2.0.

#include "../../include/stdio.h"
#include "../../include/stdlib.h"
#include "../../include/unistd.h"
#include "../../include/orionx/syscall.h"

extern "C" int fclose(FILE* file)
{
	if(!file || file->__fd < 4)
		return EOF;

	close((int) file->__fd);
	free(file);
	return 0;
}
