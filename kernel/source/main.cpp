// main.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

#include "krt.h"

extern "C" void kernel_main(nx::BootInfo* bootinfo)
{
	krt::util::memfill4b((uint32_t*) bootinfo->frameBuffer, 0x00FF0000, 800 * 600);

	while(true)
		;
}