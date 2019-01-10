// console.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "misc/stbtt.h"

namespace nx {
namespace console
{
	#if 0
	namespace fallback
	{
		int getxpos();
		int getypos();
	}

	static constexpr addr_t Framebuffer = addrs::KERNEL_FRAMEBUFFER;

	static constexpr int Padding = 6;

	static bool Initialised = false;

	static int FramebufferWidth = 0;
	static int FramebufferHeight = 0;
	static int FramebufferScanWidth = 0;

	// static int VT_Width = 0;
	// static int VT_Height = 0;

	static int CursorX = 0;
	static int CursorY = 0;

	static uint32_t CurrentFGColour = 0;
	static uint32_t CurrentBGColour = 0;


	// font rendering stuff.
	// refer to here: https://www.freetype.org/freetype2/docs/glyphs/glyphs-3.html
	// for how we (crudely) render the glyphs in the correct (ish) locations.

	struct Glpyh
	{
		uint32_t codepoint;
		int glyphIdx;

		uint32_t* bitmap;
		int width;
		int height;

		int lsb;        // the offset from the left edge of the box to the left edge of the actual glyph
		int ascent;     // how much the glyph pokes out above the baselane
		int descent;    // same but below the baseline

		int horzAdv;    // how much to advance to get from the left of one box to the left of the next.
		int vertAdv;    // same but vertically
	};

	struct Font
	{
		stbtt_fontinfo fontinfo;

		// map from unicode codepoints to the
		// treemap<uint32_t, Glpyh> glyphs;
		array<Glpyh> glyphs;
	};

	static Font MainFont;
	static float FontScale;


	// since we are working with fonts, (x, y) is the origin point of the glyph!
	// it is *not* the top-left corner of the character.
	static void draw(Glpyh* c, int x, int y, uint32_t fg, uint32_t bg)
	{
		if(c->codepoint == ' ' || c->codepoint > 126)
		{
			for(int i = y; i < (y + c->height); i++)
				krt::util::memfill4b((uint32_t*) (Framebuffer + (i * FramebufferScanWidth + x) * 4), bg, c->width);
		}
		else
		{
			// start at the origin
			uint32_t* _ptr = (uint32_t*) (Framebuffer + (y * FramebufferScanWidth + x) * 4);

			// move up by the ascent
			auto ptr = _ptr - (c->ascent * FramebufferScanWidth);

			// move right by the left-side bearing
			ptr += c->lsb;

			// now we are at the top-left of the glyph & we can start drawing.
			for(int row = 0; row < c->height; row++)
			{
				auto xptr = ptr;
				for(int col = 0; col < c->width; col++)
				{
					*xptr = c->bitmap[row * c->width + col] | bg;
					xptr++;
				}

				ptr += FramebufferScanWidth;
			}
		}
	}
	#endif


	void init(int x, int y, int scanx)
	{
		// refuse
		return;

		#if 0
		FramebufferWidth = x;
		FramebufferHeight = y;

		FramebufferScanWidth = scanx;

		// VT_Width = (FramebufferWidth - 2*Padding) / CharWidth;
		// VT_Height = (FramebufferHeight - 2*Padding) / CharHeight;

		CursorX = fallback::getxpos();
		CursorY = fallback::getypos() + 60;

		CurrentFGColour = 0xE0E0E0;
		CurrentBGColour = 0x080808;

		FontScale = 16;

		// load the font
		{
			using namespace vfs;
			File* f = open("/initrd/fonts/mono.ttf", Mode::Read);
			auto sz = stat(f).fileSize;

			auto buf = new uint8_t[sz];
			auto r = read(f, buf, sz);
			if(r != sz) abort("failed to read font file! expected %zu bytes, only %zu were read", sz, r);

			MainFont = Font();
			if(!stbtt_InitFont(&MainFont.fontinfo, buf, 0))
				abort("failed to initialise stb_truetype!");

			auto fi = &MainFont.fontinfo;

			float scale = stbtt_ScaleForPixelHeight(fi, FontScale);

			// pre-fill 128 things
			for(int i = 0; i < 128; i++)
				MainFont.glyphs.append(Glpyh());


			// preload the printable ascii region.
			for(int i = 33; i < 127; i++)
			{
				auto g = &MainFont.glyphs[i];

				g->codepoint = i;
				g->glyphIdx = stbtt_FindGlyphIndex(fi, i);



				{
					int w, h, xo, yo;

					constexpr int edge = 240;
					auto sdf = stbtt_GetGlyphSDF(fi, scale, g->glyphIdx, 0, edge, 8, &w, &h, &xo, &yo);

					g->width = w;
					g->height = h;

					g->lsb = xo;
					g->ascent = -yo;

					g->bitmap = new uint32_t[g->width * g->height];

					// now convert it to 32bpp, duplicating the value for each channel (but leaving alpha at 255)
					auto conv = [](uint8_t x) -> uint32_t {
						if(x >= edge)           x = 255;
						else if(x >= edge-1)    x = 225;
						else if(x >= edge-2)    x = 200;
						else                    x = 0;

						return 0xFF000000 | (x << 16) | (x << 8) | (x);
					};


					for(int i = 0; i < h; i++)
					{
						for(int j = 0; j < w; j++)
						{
							g->bitmap[i * w + j] = conv(sdf[i * w + j]);
						}
					}

					stbtt_FreeSDF(sdf, 0);
				}

				// get the font metrics
				{
					int asc, dsc, gap;
					stbtt_GetFontVMetrics(fi, &asc, &dsc, &gap);
					g->vertAdv = (asc - dsc + gap) * scale;
				}
				{
					int adv, lsb;
					stbtt_GetGlyphHMetrics(fi, g->glyphIdx, &adv, &lsb);
					g->horzAdv = adv * scale;
					g->lsb = lsb * scale;
				}


			}


			Initialised = true;
			halt();
		}
		#endif
	}


	void putchar(char c)
	{
		fallback::putchar(c);
	}
}
}











