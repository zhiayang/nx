// support.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "../include/stdint.h"
#include "../include/stdio.h"
#include "../include/errno.h"
#include "../include/stdlib.h"

static FILE _stdin;
static FILE _stdout;
static FILE _stderr;
static FILE _stdlog;

#define BUFFER_SIZE		4096

namespace Heap
{
	void Initialise();
}

extern "C" void init_libc()
{
	// init file descriptors
	_stdin.__fd		= 0;
	_stdout.__fd	= 1;
	_stderr.__fd	= 2;
	_stdlog.__fd	= 3;

	stdin	= &_stdin;
	stdout	= &_stdout;
	stderr	= &_stderr;
	stdlog	= &_stdlog;

	// init heap
	Heap::Initialise();

	errno = 0;

	// set the proper properties.
	stdin->buffer = (uint8_t*) malloc(BUFSIZ);
	stdin->buffersize = BUFSIZ;
	stdin->bufmode = _IOLBF | __BUFMODE_BLOCK;
}














