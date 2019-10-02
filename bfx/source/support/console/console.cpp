// console.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "bfx.h"
#include "string.h"

namespace bfx {
namespace console
{
	extern const uint8_t TheFont[256][16];

	constexpr int FontWidth = 8;
	constexpr int FontHeight = 16;

	constexpr int CharWidth = 9;
	constexpr int CharHeight = 16;

	constexpr int Padding = 6;

	static int FramebufferWidth = 0;
	static int FramebufferHeight = 0;

	// only used for memcopy operations.
	static int FramebufferScanWidth = 0;

	static int VT_Width = 0;
	static int VT_Height = 0;

	static int CursorX = 0;
	static int CursorY = 0;

	static uint64_t FramebufferAddr = 0;
	static uint32_t CurrentFGColour = 0;
	static uint32_t CurrentBGColour = 0;

	void init(void* fb, int x, int y, int scanx)
	{
		FramebufferAddr = (uint64_t) fb;

		FramebufferWidth = x;
		FramebufferHeight = y;

		FramebufferScanWidth = scanx;

		VT_Width = (FramebufferWidth - 2*Padding) / CharWidth;
		VT_Height = (FramebufferHeight - 2*Padding) / CharHeight;

		CurrentFGColour = 0xE0E0E0;
		CurrentBGColour = 0x080808;

		// and clear!
		for(size_t i = 0; i < FramebufferScanWidth * FramebufferHeight; i++)
			((uint32_t*) FramebufferAddr)[i] = CurrentBGColour;
	}


	// the x/y coords here are in pixel units!
	static void draw(char c, int x, int y, uint32_t fg, uint32_t bg)
	{
		if(!c) return;

		x += Padding;
		y += Padding;

		if(c == ' ' || c > 126)
		{
			for(int i = y; i < (y + CharHeight); i++)
			{
				for(size_t w = 0; w < CharWidth; w++)
					((uint32_t*) (FramebufferAddr + (i * FramebufferScanWidth + x) * 4))[w] = bg;
			}
		}

		uint32_t* rowAddress = (uint32_t *) FramebufferAddr + x + (y * FramebufferScanWidth);
		for(int row = 0; row < FontHeight; row++)
		{
			uint8_t data = TheFont[(int) c][row];
			uint32_t* columnAddress = rowAddress;

			int col = 0;
			for(data = TheFont[(int) c][row]; col < FontWidth; data <<= 1, col++)
			{
				if((data & 0x80) != 0)  *columnAddress = fg;
				else                    *columnAddress = bg;

				columnAddress++;
			}

			rowAddress += FramebufferScanWidth;
		}
	}

	static void scrollIfNecessary()
	{
		if(CursorY == VT_Height)
		{
			// copy.
			memmove((void*) (FramebufferAddr + (4 * FramebufferScanWidth * Padding)),
				(void*) (FramebufferAddr + (4 * FramebufferScanWidth * (Padding + CharHeight))),
				((VT_Height - 1) * CharHeight) * FramebufferScanWidth * 4);

			// memset the last row to 0.
			for(size_t i = 0; i < FramebufferScanWidth * CharHeight; i++)
			{
				((uint32_t*) (FramebufferAddr + (Padding + ((VT_Height - 1) * CharHeight))
					* FramebufferScanWidth * 4))[i] = CurrentBGColour;
			}

			CursorY -= 1;
		}
	}

	void putchar(char c)
	{
		if(c < 32 || c > 126)
		{
			if(c == '\r')
			{
				CursorX = 0;
			}
			else if(c == '\n')
			{
				CursorX = 0;
				CursorY += 1;

				scrollIfNecessary();
			}
			else if(c == '\b')
			{
				if(CursorX > 0)
				{
					CursorX -= 1;
					draw(' ', CursorX * CharWidth, CursorY * CharHeight, CurrentFGColour, CurrentBGColour);
				}
				else if(CursorY > 0)
				{
					CursorY -= 1;
					CursorX = VT_Width - 1;
					draw(' ', CursorX * CharWidth, CursorY * CharHeight, CurrentFGColour, CurrentBGColour);
				}
			}
		}
		else
		{
			// draw the current char at the current place
			draw(c, CursorX * CharWidth, CursorY * CharHeight, CurrentFGColour, CurrentBGColour);

			// move the cursor.
			CursorX += 1;

			if(CursorX == VT_Width)
			{
				// move down.
				CursorX = 0;
				CursorY += 1;

				scrollIfNecessary();
			}
		}
	}
}
}









