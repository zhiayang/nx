// fstat.cpp
// Copyright (c) 2014 - 2016, zhiayang
// Licensed under the Apache License Version 2.0.

#include "../../include/sys/stat.h"
#include "../../include/stdio.h"
#include "../../include/orionx/syscall.h"


extern "C" int stat(const char* path, struct stat* buf)
{
	// a bit shitty.
	FILE* fd = fopen(path, "r");
	int ret = Library::SystemCall::Stat(fd->__fd, buf, false);
	fclose(fd);
	return ret;
}

extern "C" int fstat(int fd, struct stat* buf)
{
	return Library::SystemCall::Stat(fd, buf, false);
}

extern "C" int lstat(const char* path, struct stat* buf)
{
	// a bit shitty.
	FILE* fd = fopen(path, "r");
	int ret = Library::SystemCall::Stat(fd->__fd, buf, true);
	fclose(fd);
	return ret;
}
