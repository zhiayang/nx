// console.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "misc/stb/stbtt.h"

namespace nx {
namespace console
{
	constexpr addr_t Framebuffer = addrs::KERNEL_FRAMEBUFFER;
	constexpr int Padding = 4;

	static bool Initialised = false;

	static int FramebufferWidth = 0;
	static int FramebufferHeight = 0;
	static int FramebufferScanWidth = 0;

	static int VT_Height = 0;

	static int CursorX = 0;
	static int CursorY = 0;

	static uint32_t CurrentFGColour = 0;
	static uint32_t CurrentBGColour = 0;

	static nx::mutex cursorLock;

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

		int topOffset;

		int lsb;        // the offset from the left edge of the box to the left edge of the actual glyph

		int horzAdv;    // how much to advance to get from the left of one box to the left of the next.
	};

	struct Font
	{
		int vertAdv;    // line height, basically
		int baseline;   // the offset from the top edge of the glyph's baseline

		stbtt_fontinfo fontinfo;
		treemap<uint32_t, Glpyh> glyphs;
	};

	static Font MainFont;
	static float FontScale = 0;

	// since we are working with fonts, (x, y) is the origin point of the glyph!
	// it is *not* the top-left corner of the character.
	static void draw(Glpyh* c, int x, int y, uint32_t fg, uint32_t bg)
	{
		// start at the origin
		uint32_t* ptr = (uint32_t*) (Framebuffer + (y * FramebufferScanWidth + x) * 4);

		// ptr -= (c->ascent * FramebufferScanWidth);
		ptr += ((MainFont.baseline + c->topOffset) * FramebufferScanWidth);

		// move right by the left-side bearing
		ptr += c->lsb;

		if(c->codepoint == ' ' || c->codepoint > 126)
		{
			for(int i = y; i < (y + MainFont.vertAdv); i++)
				krt::util::memfill4b((uint32_t*) (Framebuffer + (i * FramebufferScanWidth + x) * 4), bg, c->horzAdv);
		}
		else
		{
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


	void init(int x, int y, int scanx)
	{
		FramebufferWidth = x;
		FramebufferHeight = y;

		FramebufferScanWidth = scanx;

		// VT_Width = (FramebufferWidth - 2*Padding) / CharWidth;
		// VT_Height = (FramebufferHeight - 2*Padding) / CharHeight;

		CursorX = Padding;
		CursorY = Padding;

		CurrentFGColour = 0xE0E0E0;
		CurrentBGColour = 0x080808;

		FontScale = 24;

		cursorLock = nx::mutex();


		// load the font
		{
			using namespace vfs;
			File* f = open("/initrd/fonts/mono-bold.ttf", Mode::Read);
			assert(f);

			auto sz = stat(f).fileSize;

			auto buf = new uint8_t[sz];
			auto r = read(f, buf, sz);
			if(r != sz) abort("failed to read font file! expected %zu bytes, only %zu were read", sz, r);

			MainFont = Font();

			if(!stbtt_InitFont(&MainFont.fontinfo, buf, 0))
				abort("failed to initialise stb_truetype!");

			auto fi = &MainFont.fontinfo;

			int oversample = 1;
			float scale = stbtt_ScaleForPixelHeight(fi, FontScale) * oversample;

			// get the font metrics
			{
				int asc, dsc, gap;
				stbtt_GetFontVMetrics(fi, &asc, &dsc, &gap);
				MainFont.vertAdv = ((asc - dsc + gap) * scale) / oversample;
				MainFont.baseline = (asc * scale) / oversample;
			}



			// preload the printable ascii region.
			for(int i = 32; i < 127; i++)
			{
				auto g = Glpyh();

				g.codepoint = i;
				g.glyphIdx = stbtt_FindGlyphIndex(fi, i);

				{
					int x0, x1, y0, y1;
					stbtt_GetGlyphBitmapBox(fi, g.glyphIdx, scale, scale, &x0, &y0, &x1, &y1);

					int w = __abs(x1 - x0);
					int h = __abs(y1 - y0);

					g.width = w / oversample;
					g.height = h / oversample;

					auto bitmap = new uint8_t[(w + 1) * (h + 1)];

					stbtt_MakeGlyphBitmap(fi, bitmap, w, h, w, scale, scale, g.glyphIdx);

					// do some filtering... i guess.
					auto filter = [](uint8_t* input, double u, double v, int w, int h) -> uint32_t {
						assert(u >= 0 && u <= 1);
						assert(v >= 0 && v <= 1);

						auto lerp = [](double a, double b, double t) -> double {
							return a + (b - a) * t;
						};

						auto blerp = [&lerp](double a, double b, double c, double d, double tx, double ty) -> double {
							return lerp(lerp(a, b, tx), lerp(c, d, tx), ty);
						};

						u *= w;
						v *= h;

						int x = (int) u;
						int y = (int) v;

						if(x >= w) return 0;
						if(y >= h) return 0;

						auto a = input[(x + 0) + (y + 0) * w];
						auto b = input[(x + 1) + (y + 0) * w];
						auto c = input[(x + 0) + (y + 1) * w];
						auto d = input[(x + 1) + (y + 1) * w];

						auto val = blerp(a, b, c, d, u - x, v - y);
						auto byte = (uint8_t) val;

						return (0xFF000000 | (byte << 16) | (byte << 8) | byte);
					};

					g.bitmap = new uint32_t[g.width * g.height];
					for(int x = 0; x < g.width; x++)
					{
						for(int y = 0; y < g.height; y++)
						{
							g.bitmap[x + (y * g.width)] = filter(bitmap, (double) x / (double) g.width, (double) y / (double) g.height, w, h);
						}
					}

					delete[] bitmap;

					g.topOffset = y0 / oversample;
				}

				{
					int adv, lsb;
					stbtt_GetGlyphHMetrics(fi, g.glyphIdx, &adv, &lsb);
					g.horzAdv = (adv * scale) / oversample;
					g.lsb = (lsb * scale) / oversample;
				}

				MainFont.glyphs.insert(i, g);
			}
		}

		Initialised = true;
		VT_Height = (FramebufferHeight - 2*Padding) / MainFont.vertAdv;

		// clear it
		krt::util::memfill4b((uint32_t*) Framebuffer, CurrentBGColour, FramebufferScanWidth * FramebufferHeight);
	}

	static void scrollIfNecessary()
	{
		if(CursorY + MainFont.vertAdv + Padding >= FramebufferHeight)
		{
			// copy.
			memmove((void*) (Framebuffer + (Padding * FramebufferScanWidth * 4)),
				(void*) (Framebuffer + (4 * FramebufferScanWidth * (Padding + MainFont.vertAdv))),
				((VT_Height - 1) * MainFont.vertAdv) * FramebufferScanWidth * 4);

			// memset the last row to 0.
			krt::util::memfill4b((uint32_t*) (Framebuffer + (Padding + ((VT_Height - 1) * MainFont.vertAdv)) * FramebufferScanWidth * 4),
				CurrentBGColour, FramebufferScanWidth * MainFont.vertAdv);

			CursorY -= MainFont.vertAdv;
		}
	}

	void putchar(char c)
	{
		if(__unlikely(!Initialised))
		{
			fallback::putchar(c);
		}
		else
		{
			autolock lk(&cursorLock);

			if(c == '\r')
			{
				CursorX = Padding;
			}
			else if(c == '\n')
			{
				CursorX = Padding;
				CursorY += MainFont.vertAdv;

				scrollIfNecessary();
			}
			else if(c >= 32 && c < 127)
			{
				auto g = &MainFont.glyphs[c];
				draw(g, CursorX, CursorY, CurrentFGColour, CurrentBGColour);

				CursorX += g->horzAdv;

				if(CursorX + Padding >= FramebufferWidth)
					CursorY += MainFont.vertAdv, CursorX = Padding;

				scrollIfNecessary();
			}
		}
	}
}
}

