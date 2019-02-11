// syscall_macros.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <stddef.h>

#if !defined(__x86_64__)
	#error "don't include this file!"
#endif


#define __nx_syscall_0(num) do {                        \
	asm volatile("                                      \
		movq %0, %%rax;                                 \
		syscall                                         \
	" :: "n"(num) : "rax", "rcx", "rdx",                \
					"rdi", "rsi", "r8",                 \
					"r9",  "r10", "r11");               \
	} while(0)


#define __nx_syscall_1(num, a) do {                     \
	register uint64_t rdi asm("rdi") = (uint64_t) a;    \
	asm volatile("                                      \
		movq %0, %%rax;                                 \
		syscall                                         \
	" :: "n"(num), "r"(rdi) :                           \
					"rax", "rcx", "rdx",                \
					"rsi", "r8",  "r9",                 \
					"r10", "r11");                      \
	} while(0)

#define __nx_syscall_2(num, a, b) do {                  \
	register uint64_t rdi asm("rdi") = (uint64_t) a;    \
	register uint64_t rsi asm("rsi") = (uint64_t) b;    \
	asm volatile("                                      \
		movq %0, %%rax;                                 \
		syscall                                         \
	" :: "n"(num), "r"(rdi), "r"(rsi) :                 \
					"rax", "rcx", "rdx",                \
					"r8",  "r9",  "r10",                \
					"r11");                             \
	} while(0)

#define __nx_syscall_3(num, a, b, c) do {               \
	register uint64_t rdi asm("rdi") = (uint64_t) a;    \
	register uint64_t rsi asm("rsi") = (uint64_t) b;    \
	register uint64_t rdx asm("rdx") = (uint64_t) c;    \
	asm volatile("                                      \
		movq %0, %%rax;                                 \
		syscall                                         \
	" :: "n"(num), "r"(rdi), "r"(rsi), "r"(rdx) :       \
					"rax", "rcx", "r8",                 \
					"r9",  "r10", "r11");               \
	} while(0)

#define __nx_syscall_4(num, a, b, c, d) do {            \
	register uint64_t rdi asm("rdi") = (uint64_t) a;    \
	register uint64_t rsi asm("rsi") = (uint64_t) b;    \
	register uint64_t rdx asm("rdx") = (uint64_t) c;    \
	register uint64_t r10 asm("r10") = (uint64_t) d;    \
	asm volatile("                                      \
		movq %0, %%rax;                                 \
		syscall                                         \
	" :: "n"(num), "r"(rdi), "r"(rsi), "r"(rdx)         \
				,  "r"(r10) :                           \
					"rax", "rcx", "r8",                 \
					"r9",  "r11");                      \
	} while(0)

#define __nx_syscall_5(num, a, b, c, d, e) do {         \
	register uint64_t rdi asm("rdi") = (uint64_t) a;    \
	register uint64_t rsi asm("rsi") = (uint64_t) b;    \
	register uint64_t rdx asm("rdx") = (uint64_t) c;    \
	register uint64_t r10 asm("r10") = (uint64_t) d;    \
	register uint64_t r8  asm("r8")  = (uint64_t) e;    \
	asm volatile("                                      \
		movq %0, %%rax;                                 \
		syscall                                         \
	" :: "n"(num), "r"(rdi), "r"(rsi), "r"(rdx)         \
				,  "r"(r10), "r"(r8) :                  \
					"rax", "rcx", "r9",                 \
					"r11");                             \
	} while(0)

#define __nx_syscall_6(num, a, b, c, d, e, f) do {      \
	register uint64_t rdi asm("rdi") = (uint64_t) a;    \
	register uint64_t rsi asm("rsi") = (uint64_t) b;    \
	register uint64_t rdx asm("rdx") = (uint64_t) c;    \
	register uint64_t r10 asm("r10") = (uint64_t) d;    \
	register uint64_t r8  asm("r8")  = (uint64_t) e;    \
	register uint64_t r9  asm("r9")  = (uint64_t) f;    \
	asm volatile("                                      \
		movq %0, %%rax;                                 \
		syscall                                         \
	" :: "n"(num), "r"(rdi), "r"(rsi), "r"(rdx)         \
				,  "r"(r10), "r"(r8), "r"(r9) :         \
					"rax", "rcx", "r11");               \
	} while(0)












#define __nx_intr_syscall_0(num) do {                   \
	asm volatile("                                      \
		movq %0, %%rax;                                 \
		int $0xF8                                       \
	" :: "n"(num) : "rax", "rcx", "rdx",                \
					"rdi", "rsi", "r8",                 \
					"r9",  "r10", "r11");               \
	} while(0)


#define __nx_intr_syscall_1(num, a) do {                \
	register uint64_t rdi asm("rdi") = (uint64_t) a;    \
	asm volatile("                                      \
		movq %0, %%rax;                                 \
		int $0xF8                                       \
	" :: "n"(num), "r"(rdi) :                           \
					"rax", "rcx", "rdx",                \
					"rsi", "r8",  "r9",                 \
					"r10", "r11");                      \
	} while(0)

#define __nx_intr_syscall_2(num, a, b) do {             \
	register uint64_t rdi asm("rdi") = (uint64_t) a;    \
	register uint64_t rsi asm("rsi") = (uint64_t) b;    \
	asm volatile("                                      \
		movq %0, %%rax;                                 \
		int $0xF8                                       \
	" :: "n"(num), "r"(rdi), "r"(rsi) :                 \
					"rax", "rcx", "rdx",                \
					"r8",  "r9",  "r10",                \
					"r11");                             \
	} while(0)

#define __nx_intr_syscall_3(num, a, b, c) do {          \
	register uint64_t rdi asm("rdi") = (uint64_t) a;    \
	register uint64_t rsi asm("rsi") = (uint64_t) b;    \
	register uint64_t rdx asm("rdx") = (uint64_t) c;    \
	asm volatile("                                      \
		movq %0, %%rax;                                 \
		int $0xF8                                       \
	" :: "n"(num), "r"(rdi), "r"(rsi), "r"(rdx) :       \
					"rax", "rcx", "r8",                 \
					"r9",  "r10", "r11");               \
	} while(0)

#define __nx_intr_syscall_4(num, a, b, c, d) do {       \
	register uint64_t rdi asm("rdi") = (uint64_t) a;    \
	register uint64_t rsi asm("rsi") = (uint64_t) b;    \
	register uint64_t rdx asm("rdx") = (uint64_t) c;    \
	register uint64_t r10 asm("r10") = (uint64_t) d;    \
	asm volatile("                                      \
		movq %0, %%rax;                                 \
		int $0xF8                                       \
	" :: "n"(num), "r"(rdi), "r"(rsi), "r"(rdx)         \
				,  "r"(r10) :                           \
					"rax", "rcx", "r8",                 \
					"r9",  "r11");                      \
	} while(0)

#define __nx_intr_syscall_5(num, a, b, c, d, e) do {    \
	register uint64_t rdi asm("rdi") = (uint64_t) a;    \
	register uint64_t rsi asm("rsi") = (uint64_t) b;    \
	register uint64_t rdx asm("rdx") = (uint64_t) c;    \
	register uint64_t r10 asm("r10") = (uint64_t) d;    \
	register uint64_t r8  asm("r8")  = (uint64_t) e;    \
	asm volatile("                                      \
		movq %0, %%rax;                                 \
		int $0xF8                                       \
	" :: "n"(num), "r"(rdi), "r"(rsi), "r"(rdx)         \
				,  "r"(r10), "r"(r8) :                  \
					"rax", "rcx", "r9",                 \
					"r11");                             \
	} while(0)

#define __nx_intr_syscall_6(num, a, b, c, d, e, f) do { \
	register uint64_t rdi asm("rdi") = (uint64_t) a;    \
	register uint64_t rsi asm("rsi") = (uint64_t) b;    \
	register uint64_t rdx asm("rdx") = (uint64_t) c;    \
	register uint64_t r10 asm("r10") = (uint64_t) d;    \
	register uint64_t r8  asm("r8")  = (uint64_t) e;    \
	register uint64_t r9  asm("r9")  = (uint64_t) f;    \
	asm volatile("                                      \
		movq %0, %%rax;                                 \
		int $0xF8                                       \
	" :: "n"(num), "r"(rdi), "r"(rsi), "r"(rdx)         \
				,  "r"(r10), "r"(r8), "r"(r9) :         \
					"rax", "rcx", "r11");               \
	} while(0)
