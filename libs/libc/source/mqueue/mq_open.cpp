// mq_open.cpp
// Copyright (c) 2014 - 2016, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdarg.h>
#include "../../include/mqueue.h"
#include "../../include/fcntl.h"
#include "../../include/stdlib.h"
#include "../../include/stdio.h"
#include "../../include/string.h"
#include "../../include/orionx/syscall.h"

extern "C" mqd_t mq_open(const char* path, int flags, ...)
{
	mode_t mode = 0;
	(void) mode;
	if(flags & O_CREAT)
	{
		va_list ap;
		va_start(ap, flags);
		mode = va_arg(ap, mode_t);
		va_end(ap);
	}

	// prepend the path with '/dev/_mq/' so the kernel knows how to handle this.
	auto prlen = __PREFIX_LENGTH;

	if(path == 0)
		return -1;

	if(path[0] == '/')
		prlen--;


	char* id = (char*) malloc(strlen(path) + prlen + 1);

	memcpy(id, "/dev/_mq/", prlen);
	memcpy(id + prlen, path, strlen(path));
	*(id + prlen + strlen(path) + 1) = 0;


	// TODO: flags ignored for now
	// this should actually fail if it doesn't exist, unless flags & O_CREAT.
	int fd = (int) Library::SystemCall::Open(id, flags);
	free(id);
	return fd;
}

