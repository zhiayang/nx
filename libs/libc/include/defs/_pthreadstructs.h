// _pthreadstructs.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#ifndef __pthreadstructs_h
#define __pthreadstructs_h
#include "../sys/cdefs.h"
#include "../stdint.h"
__BEGIN_DECLS

struct ThreadRegisterState_type
{
	uint64_t rdi, rsi, rbp;
	uint64_t rax, rbx, rcx, rdx;
	uint64_t r8, r9, r10, r11, r12, r13, r14, r15;

	uint64_t __rsp;
};

struct Thread_attr
{
	uint8_t priority;
	uintptr_t stackptr;
	uintptr_t stacksize;

	void* a1;
	void* a2;
	void* a3;
	void* a4;
	void* a5;
	void* a6;

	struct ThreadRegisterState_type regs;
};


struct Mutex_attr
{
	uint64_t type;
};


__END_DECLS
#endif
