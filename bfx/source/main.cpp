// main.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdint.h>
#include <stddef.h>

#include "bfx.h"
#include "krt.h"

#include "bootboot.h"

#define BFX_VERSION_STRING "0.1.0"

void bfx::init(BOOTBOOT* bbinfo)
{
	println("bfx bootloader");
	println("version %s\n", BFX_VERSION_STRING);

	idt::init();
	exceptions::init();

	// start the pmm
	{
		MMapEnt* mmapEntries = &bbinfo->mmap;
		size_t numEntries = ((((uint8_t*) bbinfo) + bbinfo->size) - (uint8_t*) mmapEntries) / sizeof(MMapEnt);

		pmm::init(mmapEntries, numEntries);
	}
}

















// handled by the linker script
extern "C" BOOTBOOT bootboot;

// entry point. we are in long mode. we modified BOOTBOOT
// so it doesn't start the APs.
extern "C" void entry()
{
	bfx::println("");

	bfx::init(&bootboot);
	while(true);
}







