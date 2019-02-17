// syscall_macros.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <stddef.h>

#if !defined(__x86_64__)
	#error "don't include this file!"
#endif


#define __nx_syscall_0(num, ret) do {                           \
	asm volatile("                                              \
		movq %[n], %%rax;                                       \
		syscall                                                 \
	"   : "=a"(ret)                                             \
		: [n]"n"(num)                                           \
		: "rcx", "rdx", "rdi", "rsi", "r8", "r9",  "r10", "r11"); } while(0)

#define __nx_syscall_1(num, ret, a) do {                        \
	register uint64_t rdi asm("rdi") = (uint64_t) a;            \
	asm volatile("                                              \
		movq %[n], %%rax;                                       \
		syscall                                                 \
	"   : "=a"(ret)                                             \
		: [n]"n"(num), "r"(rdi)                                 \
		: "rcx", "rdx", "rsi", "r8", "r9",  "r10", "r11"); } while(0)

#define __nx_syscall_2(num, ret, a, b) do {                     \
	register uint64_t rdi asm("rdi") = (uint64_t) a;            \
	register uint64_t rsi asm("rsi") = (uint64_t) b;            \
	asm volatile("                                              \
		movq %[n], %%rax;                                       \
		syscall                                                 \
	"   : "=a"(ret)                                             \
		: [n]"n"(num), "r"(rdi), "r"(rsi)                       \
		: "rcx", "rdx", "r8", "r9", "r10",  "r11"); } while(0)

#define __nx_syscall_3(num, ret, a, b, c) do {                  \
	register uint64_t rdi asm("rdi") = (uint64_t) a;            \
	register uint64_t rsi asm("rsi") = (uint64_t) b;            \
	register uint64_t rdx asm("rdx") = (uint64_t) c;            \
	asm volatile("                                              \
		movq %[n], %%rax;                                       \
		syscall                                                 \
	"   : "=a"(ret)                                             \
		: [n]"n"(num), "r"(rdi), "r"(rsi), "r"(rdx)             \
		: "rcx", "r8",  "r9", "r10",  "r11"); } while(0)

#define __nx_syscall_4(num, ret, a, b, c, d) do {               \
	register uint64_t rdi asm("rdi") = (uint64_t) a;            \
	register uint64_t rsi asm("rsi") = (uint64_t) b;            \
	register uint64_t rdx asm("rdx") = (uint64_t) c;            \
	register uint64_t r10 asm("r10") = (uint64_t) d;            \
	asm volatile("                                              \
		movq %[n], %%rax;                                       \
		syscall                                                 \
	"   : "=a"(ret)                                             \
		: [n]"n"(num), "r"(rdi), "r"(rsi), "r"(rdx), "r"(r10)   \
		: "rcx", "r8", "r9", "r11"); } while(0)

#define __nx_syscall_5(num, ret, a, b, c, d, e) do {            \
	register uint64_t rdi asm("rdi") = (uint64_t) a;            \
	register uint64_t rsi asm("rsi") = (uint64_t) b;            \
	register uint64_t rdx asm("rdx") = (uint64_t) c;            \
	register uint64_t r10 asm("r10") = (uint64_t) d;            \
	register uint64_t r8  asm("r8")  = (uint64_t) e;            \
	asm volatile("                                              \
		movq %[n], %%rax;                                       \
		syscall                                                 \
	"   : "=a"(ret)                                             \
		: [n]"n"(num), "r"(rdi), "r"(rsi), "r"(rdx), "r"(r10), "r"(r8) \
		: "rcx", "r9", "r11"); } while(0)

#define __nx_syscall_6(num, ret, a, b, c, d, e, f) do {         \
	register uint64_t rdi asm("rdi") = (uint64_t) a;            \
	register uint64_t rsi asm("rsi") = (uint64_t) b;            \
	register uint64_t rdx asm("rdx") = (uint64_t) c;            \
	register uint64_t r10 asm("r10") = (uint64_t) d;            \
	register uint64_t r8  asm("r8")  = (uint64_t) e;            \
	register uint64_t r9  asm("r9")  = (uint64_t) f;            \
	asm volatile("                                              \
		movq %[n], %%rax;                                       \
		syscall                                                 \
	"   : "=a"(ret)                                             \
		: [n]"n"(num), "r"(rdi), "r"(rsi), "r"(rdx), "r"(r10), "r"(r8), "r"(r9) \
		: "rcx", "r11"); } while(0)





#define __nx_syscall_0v(num) do {                               \
	asm volatile("                                              \
		movq %[n], %%rax;                                       \
		syscall                                                 \
	"   :: [n]"n"(num)                                           \
		: "rax", "rcx", "rdx", "rdi", "rsi", "r8", "r9",  "r10", "r11"); } while(0)

#define __nx_syscall_1v(num, a) do {                            \
	register uint64_t rdi asm("rdi") = (uint64_t) a;            \
	asm volatile("                                              \
		movq %[n], %%rax;                                       \
		syscall                                                 \
	"   :: [n]"n"(num), "r"(rdi)                                 \
		: "rax", "rcx", "rdx", "rsi", "r8", "r9",  "r10", "r11"); } while(0)

#define __nx_syscall_2v(num, a, b) do {                         \
	register uint64_t rdi asm("rdi") = (uint64_t) a;            \
	register uint64_t rsi asm("rsi") = (uint64_t) b;            \
	asm volatile("                                              \
		movq %[n], %%rax;                                       \
		syscall                                                 \
	"   :: [n]"n"(num), "r"(rdi), "r"(rsi)                       \
		: "rax", "rcx", "rdx", "r8", "r9", "r10",  "r11"); } while(0)

#define __nx_syscall_3v(num, a, b, c) do {                      \
	register uint64_t rdi asm("rdi") = (uint64_t) a;            \
	register uint64_t rsi asm("rsi") = (uint64_t) b;            \
	register uint64_t rdx asm("rdx") = (uint64_t) c;            \
	asm volatile("                                              \
		movq %[n], %%rax;                                       \
		syscall                                                 \
	"   :: [n]"n"(num), "r"(rdi), "r"(rsi), "r"(rdx)             \
		: "rax", "rcx", "r8",  "r9", "r10",  "r11"); } while(0)

#define __nx_syscall_4v(num, a, b, c, d) do {                   \
	register uint64_t rdi asm("rdi") = (uint64_t) a;            \
	register uint64_t rsi asm("rsi") = (uint64_t) b;            \
	register uint64_t rdx asm("rdx") = (uint64_t) c;            \
	register uint64_t r10 asm("r10") = (uint64_t) d;            \
	asm volatile("                                              \
		movq %[n], %%rax;                                       \
		syscall                                                 \
	"   :: [n]"n"(num), "r"(rdi), "r"(rsi), "r"(rdx), "r"(r10)   \
		: "rax", "rcx", "r8", "r9", "r11"); } while(0)

#define __nx_syscall_5v(num, a, b, c, d, e) do {                \
	register uint64_t rdi asm("rdi") = (uint64_t) a;            \
	register uint64_t rsi asm("rsi") = (uint64_t) b;            \
	register uint64_t rdx asm("rdx") = (uint64_t) c;            \
	register uint64_t r10 asm("r10") = (uint64_t) d;            \
	register uint64_t r8  asm("r8")  = (uint64_t) e;            \
	asm volatile("                                              \
		movq %[n], %%rax;                                       \
		syscall                                                 \
	"   :: [n]"n"(num), "r"(rdi), "r"(rsi), "r"(rdx), "r"(r10), "r"(r8) \
		: "rax", "rcx", "r9", "r11"); } while(0)

#define __nx_syscall_6v(num, a, b, c, d, e, f) do {             \
	register uint64_t rdi asm("rdi") = (uint64_t) a;            \
	register uint64_t rsi asm("rsi") = (uint64_t) b;            \
	register uint64_t rdx asm("rdx") = (uint64_t) c;            \
	register uint64_t r10 asm("r10") = (uint64_t) d;            \
	register uint64_t r8  asm("r8")  = (uint64_t) e;            \
	register uint64_t r9  asm("r9")  = (uint64_t) f;            \
	asm volatile("                                              \
		movq %[n], %%rax;                                       \
		syscall                                                 \
	"   :: [n]"n"(num), "r"(rdi), "r"(rsi), "r"(rdx), "r"(r10), "r"(r8), "r"(r9) \
		: "rax", "rcx", "r11"); } while(0)


































#define __nx_intr_syscall_0(num, ret) do {                      \
	asm volatile("                                              \
		movq %[n], %%rax;                                       \
		int $0xF8                                               \
	"   : "=a"(ret)                                             \
		: [n]"n"(num)                                           \
		: "rcx", "rdx", "rdi", "rsi", "r8", "r9",  "r10", "r11"); } while(0)

#define __nx_intr_syscall_1(num, ret, a) do {                   \
	register uint64_t rdi asm("rdi") = (uint64_t) a;            \
	asm volatile("                                              \
		movq %[n], %%rax;                                       \
		int $0xF8                                               \
	"   : "=a"(ret)                                             \
		: [n]"n"(num), "r"(rdi)                                 \
		: "rcx", "rdx", "rsi", "r8", "r9", "r10", "r11"); } while(0)

#define __nx_intr_syscall_2(num, ret, a, b) do {                \
	register uint64_t rdi asm("rdi") = (uint64_t) a;            \
	register uint64_t rsi asm("rsi") = (uint64_t) b;            \
	asm volatile("                                              \
		movq %[n], %%rax;                                       \
		int $0xF8                                               \
	"   : "=a"(ret)                                             \
		: [n]"n"(num), "r"(rdi), "r"(rsi)                       \
		: "rcx", "rdx", "r8", "r9", "r10", "r11"); } while(0)

#define __nx_intr_syscall_3(num, ret, a, b, c) do {             \
	register uint64_t rdi asm("rdi") = (uint64_t) a;            \
	register uint64_t rsi asm("rsi") = (uint64_t) b;            \
	register uint64_t rdx asm("rdx") = (uint64_t) c;            \
	asm volatile("                                              \
		movq %[n], %%rax;                                       \
		int $0xF8                                               \
	"   : "=a"(ret)                                             \
		: [n]"n"(num), "r"(rdi), "r"(rsi), "r"(rdx)             \
		: "rcx", "r8", "r9",  "r10", "r11"); } while(0)

#define __nx_intr_syscall_4(num, ret, a, b, c, d) do {          \
	register uint64_t rdi asm("rdi") = (uint64_t) a;            \
	register uint64_t rsi asm("rsi") = (uint64_t) b;            \
	register uint64_t rdx asm("rdx") = (uint64_t) c;            \
	register uint64_t r10 asm("r10") = (uint64_t) d;            \
	asm volatile("                                              \
		movq %[n], %%rax;                                       \
		int $0xF8                                               \
	"   : "=a"(ret)                                             \
		: [n]"n"(num), "r"(rdi), "r"(rsi), "r"(rdx), "r"(r10)   \
		: "rcx", "r8", "r9",  "r11"); } while(0)

#define __nx_intr_syscall_5(num, ret, a, b, c, d, e) do {       \
	register uint64_t rdi asm("rdi") = (uint64_t) a;            \
	register uint64_t rsi asm("rsi") = (uint64_t) b;            \
	register uint64_t rdx asm("rdx") = (uint64_t) c;            \
	register uint64_t r10 asm("r10") = (uint64_t) d;            \
	register uint64_t r8  asm("r8")  = (uint64_t) e;            \
	asm volatile("                                              \
		movq %[n], %%rax;                                       \
		int $0xF8                                               \
	"   : "=a"(ret)                                             \
		: [n]"n"(num), "r"(rdi), "r"(rsi), "r"(rdx), "r"(r10), "r"(r8) \
		: "rcx", "r9", "r11"); } while(0)

#define __nx_intr_syscall_6(num, ret, a, b, c, d, e, f) do {    \
	register uint64_t rdi asm("rdi") = (uint64_t) a;            \
	register uint64_t rsi asm("rsi") = (uint64_t) b;            \
	register uint64_t rdx asm("rdx") = (uint64_t) c;            \
	register uint64_t r10 asm("r10") = (uint64_t) d;            \
	register uint64_t r8  asm("r8")  = (uint64_t) e;            \
	register uint64_t r9  asm("r9")  = (uint64_t) f;            \
	asm volatile("                                              \
		movq %[n], %%rax;                                       \
		int $0xF8                                               \
	"   : "=a"(ret)                                             \
		: [n]"n"(num), "r"(rdi), "r"(rsi), "r"(rdx), "r"(r10), "r"(r8), "r"(r9) \
		: "rcx", "r11"); } while(0)










#define __nx_intr_syscall_0v(num) do {                          \
	asm volatile("                                              \
		movq %[n], %%rax;                                       \
		int $0xF8                                               \
	"   :: [n]"n"(num)                                           \
		: "rax", "rcx", "rdx", "rdi", "rsi", "r8", "r9",  "r10", "r11"); } while(0)

#define __nx_intr_syscall_1v(num, a) do {                       \
	register uint64_t rdi asm("rdi") = (uint64_t) a;            \
	asm volatile("                                              \
		movq %[n], %%rax;                                       \
		int $0xF8                                               \
	"   :: [n]"n"(num), "r"(rdi)                                 \
		: "rax", "rcx", "rdx", "rsi", "r8", "r9", "r10", "r11"); } while(0)

#define __nx_intr_syscall_2v(num, a, b) do {                    \
	register uint64_t rdi asm("rdi") = (uint64_t) a;            \
	register uint64_t rsi asm("rsi") = (uint64_t) b;            \
	asm volatile("                                              \
		movq %[n], %%rax;                                       \
		int $0xF8                                               \
	"   :: [n]"n"(num), "r"(rdi), "r"(rsi)                       \
		: "rax", "rcx", "rdx", "r8", "r9", "r10", "r11"); } while(0)

#define __nx_intr_syscall_3v(num, a, b, c) do {                 \
	register uint64_t rdi asm("rdi") = (uint64_t) a;            \
	register uint64_t rsi asm("rsi") = (uint64_t) b;            \
	register uint64_t rdx asm("rdx") = (uint64_t) c;            \
	asm volatile("                                              \
		movq %[n], %%rax;                                       \
		int $0xF8                                               \
	"   :: [n]"n"(num), "r"(rdi), "r"(rsi), "r"(rdx)             \
		: "rax", "rcx", "r8", "r9",  "r10", "r11"); } while(0)

#define __nx_intr_syscall_4v(num, a, b, c, d) do {              \
	register uint64_t rdi asm("rdi") = (uint64_t) a;            \
	register uint64_t rsi asm("rsi") = (uint64_t) b;            \
	register uint64_t rdx asm("rdx") = (uint64_t) c;            \
	register uint64_t r10 asm("r10") = (uint64_t) d;            \
	asm volatile("                                              \
		movq %[n], %%rax;                                       \
		int $0xF8                                               \
	"   :: [n]"n"(num), "r"(rdi), "r"(rsi), "r"(rdx), "r"(r10)   \
		: "rax", "rcx", "r8", "r9",  "r11"); } while(0)

#define __nx_intr_syscall_5v(num, a, b, c, d, e) do {           \
	register uint64_t rdi asm("rdi") = (uint64_t) a;            \
	register uint64_t rsi asm("rsi") = (uint64_t) b;            \
	register uint64_t rdx asm("rdx") = (uint64_t) c;            \
	register uint64_t r10 asm("r10") = (uint64_t) d;            \
	register uint64_t r8  asm("r8")  = (uint64_t) e;            \
	asm volatile("                                              \
		movq %[n], %%rax;                                       \
		int $0xF8                                               \
	"   :: [n]"n"(num), "r"(rdi), "r"(rsi), "r"(rdx), "r"(r10), "r"(r8) \
		: "rax", "rcx", "r9", "r11"); } while(0)

#define __nx_intr_syscall_6v(num, a, b, c, d, e, f) do {        \
	register uint64_t rdi asm("rdi") = (uint64_t) a;            \
	register uint64_t rsi asm("rsi") = (uint64_t) b;            \
	register uint64_t rdx asm("rdx") = (uint64_t) c;            \
	register uint64_t r10 asm("r10") = (uint64_t) d;            \
	register uint64_t r8  asm("r8")  = (uint64_t) e;            \
	register uint64_t r9  asm("r9")  = (uint64_t) f;            \
	asm volatile("                                              \
		movq %[n], %%rax;                                       \
		int $0xF8                                               \
	"   :: [n]"n"(num), "r"(rdi), "r"(rsi), "r"(rdx), "r"(r10), "r"(r8), "r"(r9) \
		: "rax", "rcx", "r11"); } while(0)







