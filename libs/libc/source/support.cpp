// support.cpp
// Copyright (c) 2014 - 2016, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

static FILE _stdin;
static FILE _stdout;
static FILE _stderr;

#define BUFFER_SIZE		4096

namespace heap
{
	void init();
}

extern "C" void init_libc()
{
	// init file descriptors
	_stdin.__fd		= 0;
	_stdout.__fd	= 1;
	_stderr.__fd	= 2;

	stdin	= &_stdin;
	stdout	= &_stdout;
	stderr	= &_stderr;

	heap::init();

	errno = 0;

	// set the proper properties.
	stdin->buffer = (uint8_t*) malloc(BUFSIZ);
	stdin->buffersize = BUFSIZ;
	stdin->bufmode = _IOLBF | __BUFMODE_BLOCK;
}














