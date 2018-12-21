// setjmp.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.


#include "stdint.h"
#include "signal.h"

#ifndef __pthread_h
#define __pthread_h

#ifdef __cplusplus
extern "C" {
#endif

// typedef unsigned long sigjmp_buf[8 + 1 + __SIGCOUNT / (sizeof(unsigned long) * 8)];
// typedef sigjmp_buf jmp_buf;

struct __jmp_buf_regs
{
	uint64_t rdi;
	uint64_t rsi;
	uint64_t rbp;

	uint64_t rax;
	uint64_t rbx;
	uint64_t rcx;
	uint64_t rdx;

	uint64_t r8;
	uint64_t r9;
	uint64_t r10;
	uint64_t r11;
	uint64_t r12;
	uint64_t r13;
	uint64_t r14;
	uint64_t r15;

	uint64_t ret;
	uint64_t rsp;
	uint64_t rflags;
};
typedef struct __jmp_buf_regs __jmp_buf_regs;

struct __jmp_buf
{
	// potentially other information, like signals etc.
	__jmp_buf_regs regs;
};
typedef struct __jmp_buf jmp_buf[1];

void longjmp(jmp_buf buf, int ret);
int setjmp(jmp_buf buf);

// TODO.
// void siglongjmp(sigjmp_buf, int);
// int sigsetjmp(sigjmp_buf, int);

#ifdef __cplusplus
}
#endif

#endif
