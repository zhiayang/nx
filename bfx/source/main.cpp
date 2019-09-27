// main.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdint.h>
#include <stddef.h>

#include "bootboot.h"

// imported virtual addresses, see linker script
extern BOOTBOOT bootboot;

// entry point. we are in long mode. we modified BOOTBOOT
// so it doesn't start the APs.
void _start()
{






	int x, y, s=bootboot.fb_scanline, w=bootboot.fb_width, h=bootboot.fb_height;

	// cross-hair to see screen dimension detected correctly
	for(y=0;y<h;y++) { *((uint32_t*)(bootboot.fb_ptr + s*y + (w*2)))=0x00FFFFFF; }
	for(x=0;x<w;x++) { *((uint32_t*)(bootboot.fb_ptr + s*(h/2)+x*4))=0x00FFFFFF; }

	// red, green, blue boxes in order
	for(y=0;y<20;y++) { for(x=0;x<20;x++) { *((uint32_t*)(bootboot.fb_ptr + s*(y+20) + (x+20)*4))=0x00FF0000; } }
	for(y=0;y<20;y++) { for(x=0;x<20;x++) { *((uint32_t*)(bootboot.fb_ptr + s*(y+20) + (x+50)*4))=0x0000FF00; } }
	for(y=0;y<20;y++) { for(x=0;x<20;x++) { *((uint32_t*)(bootboot.fb_ptr + s*(y+20) + (x+80)*4))=0x000000FF; } }

	// hang for now
	while(1);
}







