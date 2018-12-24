// fopen.cpp
// Copyright (c) 2014 - 2016, zhiayang
// Licensed under the Apache License Version 2.0.

#include "../../include/stdio.h"
#include "../../include/stdlib.h"
#include "../../include/unistd.h"
#include "../../include/fcntl.h"
#include "../../include/orionx/syscall.h"

extern "C" FILE* fopen(const char* fname, const char* mode)
{
	// TODO.
	// ignore for now.
	(void) mode;

	uint64_t fd = open(fname, 0);
	if(!fd)
		return NULL;

	FILE* ret = (FILE*) malloc(sizeof(FILE));
	ret->__fd = fd;

	return ret;
}

extern "C" FILE* freopen(const char* path, const char* mode, FILE* str)
{
	// again, ignore mode
	// TODO
	(void) mode;


	close((int) str->__fd);

	// TODO: handle flags
	int fd = open(path, O_RDWR);

	if(fd)
	{
		str->__fd = fd;
		return str;
	}
	else
		return NULL;
}

extern "C" FILE* fdopen(int fd, const char* mode)
{
	// TODO: mode.
	(void) mode;

	FILE* ret = (FILE*) malloc(sizeof(FILE));
	ret->__fd = fd;

	return ret;
}

extern "C" int fileno(FILE* str)
{
	return (int) str->__fd;
}



