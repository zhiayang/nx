// exit.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.


#include "../../include/stdlib.h"
#include "../../include/stdio.h"

func_void_void_t __exitfunc = 0;

struct exit_handler
{
	func_void_void_t callback;
	exit_handler* next;
};

exit_handler* handlerStack = 0;

extern "C" int atexit(func_void_void_t func)
{
	exit_handler* top = (exit_handler*) malloc(sizeof(exit_handler));
	top->callback = func;
	top->next = handlerStack;

	handlerStack = top;
	return 0;
}



// in asm
extern "C" void _fini();
extern "C" void exit(int ret)
{
	exit_handler* h = handlerStack;
	while(h)
	{
		h->callback();
		exit_handler* oldh = h;
		h = h->next;

		free(oldh);
	}

	// flush the shits
	fflush(stdout);

	(void) ret;
	_fini();

	// do a system call
	asm volatile("xor %%r10, %%r10; int $0xF8" ::: "%r10");
	while(true);
}

extern "C" void abort()
{
	exit(-42);
}
