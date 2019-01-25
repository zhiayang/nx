#if 0

// console.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

//* fonts are generated using https://github.com/zhiayang/fontem
//* aka https://github.com/chrisy/fontem, but forked in case something happens to the original
//* no modifications are made in the fork though.
#include "font/font-dejavu-mono-10.h"
#define FONT_NAME font_dejavu_mono_10


namespace nx {
namespace console
{
	static constexpr addr_t Framebuffer = addrs::KERNEL_FRAMEBUFFER;
	static constexpr int Padding = 6;

	static int FramebufferWidth = 0;
	static int FramebufferHeight = 0;
	static int FramebufferScanWidth = 0;

	static int CursorX = 0;
	static int CursorY = 0;
	static int CursorTop = 0;

	static int VT_Height = 0;

	static uint32_t CurrentFGColour = 0;
	static uint32_t CurrentBGColour = 0;

	static constexpr int LineAdv = 16;


	struct Glpyh
	{
		uint32_t codepoint;
		uint32_t* bitmap;

		uint8_t* bitmap1b;

		int width;
		int height;

		int lsb;        // the offset from the left edge of the box to the left edge of the actual glyph
		int ascent;     // how much the glyph pokes out above the baselane

		int horzAdv;    // how much to advance to get from the left of one box to the left of the next.
		int vertAdv;    // same but vertically
	};

	static int HeightAboveBaseline;
	static int HeightBelowBaseline;

	static Glpyh Glyphs[128];

	static void (*draw_func)(Glpyh* g, int x, int y, uint32_t fg, uint32_t bg);


	static uint32_t conv(uint8_t x)
	{
		return 0xFF000000 | (x << 16) | (x << 8) | (x);
	}


	// note: we have two versions, one that's 'pre' -- before we have a heap and and make an efficient bitmap,
	// forcing us to convert the 8bpp on the fly and the non-pre, where we already created the 32bpp bitmap (after calling init_stage2())

	template <bool pre>
	static void draw(Glpyh* g, int x, int y, uint32_t fg, uint32_t bg)
	{
		if(g->codepoint == ' ' || g->codepoint > 126)
		{
			for(int i = y - HeightAboveBaseline; i < (y + HeightBelowBaseline); i++)
				krt::util::memfill4b((uint32_t*) (Framebuffer + (i * FramebufferScanWidth + x) * 4), bg, g->width);
		}
		else
		{
			// start at the origin
			uint32_t* _ptr = (uint32_t*) (Framebuffer + (y * FramebufferScanWidth + x) * 4);

			// move up by the ascent
			auto ptr = _ptr - (g->ascent * FramebufferScanWidth);

			// move right by the left-side bearing
			ptr += g->lsb;

			// now we are at the top-left of the glyph & we can start drawing.
			for(int row = 0; row < g->height; row++)
			{
				auto xptr = ptr;
				for(int col = 0; col < g->width; col++)
				{
					if constexpr (pre)  *xptr = conv(g->bitmap1b[row * g->width + col]) | bg;
					else                *xptr = g->bitmap[row * g->width + col] | bg;

					xptr++;
				}

				ptr += FramebufferScanWidth;
			}
		}
	}


	void clear()
	{
		krt::util::memfill4b((uint32_t*) Framebuffer, CurrentBGColour, FramebufferScanWidth * FramebufferHeight);

		CursorX = Padding;
		CursorY = CursorTop;
	}

	void init(int x, int y, int scanx)
	{
		FramebufferWidth = x;
		FramebufferHeight = y;
		FramebufferScanWidth = scanx;

		CurrentFGColour = 0xE0E0E0;
		CurrentBGColour = 0x080808;

		CursorTop = Padding + LineAdv;

		clear();

		memset(&Glyphs[0], 0, sizeof(Glpyh) * 128);

		auto font = FONT_NAME;

		HeightAboveBaseline = font.ascender;
		HeightBelowBaseline = font.descender;


		for(int c = 32; c < 127; c++)
		{
			auto glp = &Glyphs[c];
			auto fg = font.glyphs[c - 32];

			glp->codepoint = c;
			glp->width = fg->cols;
			glp->height = fg->rows;
			glp->bitmap1b = (uint8_t*) fg->bitmap;

			glp->lsb = fg->left;
			glp->ascent = fg->top;
			glp->horzAdv = fg->advance;
			glp->vertAdv = LineAdv;
		}

		VT_Height = FramebufferHeight / LineAdv;
		draw_func = draw<true>;
	}

	void init_stage2()
	{
		for(int c = 32; c < 127; c++)
		{
			auto glp = &Glyphs[c];

			glp->bitmap = new uint32_t[glp->width * glp->height];

			for(int i = 0; i < glp->height; i++)
			{
				for(int j = 0; j < glp->width; j++)
				{
					glp->bitmap[i * glp->width + j] = conv(glp->bitmap1b[i * glp->width + j]);
				}
			}
		}

		draw_func = draw<false>;
	}


















	static void scrollIfNecessary()
	{
		if(CursorY + LineAdv >= FramebufferHeight)
		{
			// copy.
			memmove((void*) (Framebuffer + (Padding * FramebufferScanWidth * 4)),
				(void*) (Framebuffer + (4 * FramebufferScanWidth * (CursorTop))),
				((VT_Height - 1) * LineAdv) * FramebufferScanWidth * 4);

			// memset the last row to 0.
			krt::util::memfill4b((uint32_t*) (Framebuffer + (Padding + ((VT_Height - 1) * LineAdv)) * FramebufferScanWidth * 4),
				CurrentBGColour, FramebufferScanWidth * LineAdv);

			CursorY -= LineAdv;
		}
	}

	void putchar(char c)
	{
		if(c < 32 || c > 126)
		{
			if(c == '\r')
			{
				CursorX = Padding;
			}
			else if(c == '\n')
			{
				CursorX = Padding;
				CursorY += LineAdv;

				scrollIfNecessary();
			}
		}
		else
		{
			auto g = &Glyphs[(int) c];
			draw_func(g, CursorX, CursorY, CurrentFGColour, CurrentBGColour);

			CursorX += g->horzAdv;

			// assume a monospace font.
			if(CursorX + g->horzAdv + Padding >= FramebufferWidth)
			{
				// we need to scrooool
				CursorX = Padding;
				CursorY += LineAdv;

				scrollIfNecessary();
			}
		}
	}
}
}
#endif



























