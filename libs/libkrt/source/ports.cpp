// ports.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "krt.h"

namespace krt {
namespace port
{
	uint8_t read1b(uint16_t port)
	{
		uint8_t ret;
		asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
		return ret;
	}

	uint16_t read2b(uint16_t port)
	{
		uint16_t ret;
		asm volatile("inw %1, %0" : "=a"(ret) : "Nd"(port));
		return ret;
	}

	uint32_t read4b(uint16_t port)
	{
		uint32_t ret;
		asm volatile("inl %1, %0" : "=a"(ret) : "Nd"(port));
		return ret;
	}



	void write1b(uint8_t value, uint16_t port)
	{
		asm volatile("outb %0, %1" :: "a"(value), "Nd"(port));
	}

	void write2b(uint16_t value, uint16_t port)
	{
		asm volatile("outw %0, %1" :: "a"(value), "Nd"(port));
	}

	void write4b(uint32_t value, uint16_t port)
	{
		asm volatile("outl %0, %1" :: "a"(value), "Nd"(port));
	}
}
}


























