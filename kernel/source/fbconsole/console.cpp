// console.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace console
{
	static constexpr addr_t Framebuffer = addrs::KERNEL_FRAMEBUFFER;

	static constexpr int Padding = 6;
	static constexpr int CharWidth = 8;
	static constexpr int CharHeight = 16;

	static bool Initialised = false;

	static int FramebufferWidth = 0;
	static int FramebufferHeight = 0;
	static int FramebufferScanWidth = 0;

	static int VT_Width = 0;
	static int VT_Height = 0;

	static int CursorX = 0;
	static int CursorY = 0;

	static uint32_t CurrentFGColour = 0;
	static uint32_t CurrentBGColour = 0;



	void init(int x, int y, int scanx)
	{
		FramebufferWidth = x;
		FramebufferHeight = y;

		FramebufferScanWidth = scanx;

		VT_Width = (FramebufferWidth - 2*Padding) / CharWidth;
		VT_Height = (FramebufferHeight - 2*Padding) / CharHeight;

		CurrentFGColour = 0xE0E0E0;
		CurrentBGColour = 0x101010;

		// and clear!
		krt::util::memfill4b((uint32_t*) Framebuffer, CurrentBGColour, FramebufferScanWidth * FramebufferHeight);


		// load the font
		{
			using namespace vfs;
			File* f = open("/initrd/fonts/mono.ttf", Mode::Read);
			auto sz = stat(f).fileSize;
			println("sz = %zu", sz);

			auto buf = new uint8_t[sz];
			auto r = read(f, buf, sz);
			if(r != sz) abort("failed to read font file! expected %zu bytes, only %zu were read", sz, r);

			halt();
		}
	}

	void putchar(char c)
	{
		if(!Initialised)
		{
			fallback::putchar(c);
		}
		else
		{
		}
	}
}
}











