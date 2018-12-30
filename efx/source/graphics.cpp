// graphics.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "efx.h"
#include "efi/system-table.h"
#include "efi/protocol/graphics-output.h"

namespace efx {
namespace graphics
{
	static int xResolution = 0;
	static int yResolution = 0;
	static int pixelsPerScanline = 0;

	int getX()                  { return xResolution; }
	int getY()                  { return yResolution; }
	int getPixelsPerScanline()  { return pixelsPerScanline; }


	void setDisplayMode(int defX, int defY)
	{
		int xres = 0;
		int yres = 0;
		{
			auto r = options::get_option("display");
			do {
				if(r == "")
				{
					xres = defX;
					yres = defY;
				}
				else
				{
					// oooook, parse the thing.
					// strtol gives us the pointer to the first non digit char
					char* next = 0;

					xres = strtol(r.cstr(), &next, 10);
					if(next == r.cstr() || next == r.cstr() + r.size() || next[0] != 'x')
					{
						efi::println("invalid display resolution: '%s'. expected two dimensions, eg. 123x456. ignoring and using defaults", r.cstr());

						xres = defX;
						yres = defY;
						break;
					}

					yres = strtol(next + 1, nullptr, 10);
				}
			} while(false);
		}

		if(xres < 320)
		{
			efi::println("x dimension '%d' is too small, using the minimum of 320", xres < 320 ? xres : yres);
			xres = __max(320, xres);
		}

		if(yres < 320)
		{
			efi::println("y dimension '%d' is too small, using the minimum of 320", xres < 320 ? xres : yres);
			yres = __max(320, yres);
		}


		// lets actually start.
		efi::println("selected display resolution: %dx%d", xres, yres);

		uint32_t bestMode = -1;
		efi_graphics_output_protocol* gop = 0;
		efi_graphics_output_mode_information* bestmodeinfo = 0;
		{
			auto stat = efi::systable()->BootServices->LocateProtocol(efi::guid::protoGraphicsOutput(), 0, (void**) &gop);
			efi::abort_if_error(stat, "failed to get graphics output protocol");

			// our 'heuristic' is (x + y) / ratio
			double wanted = (xres + yres) / ((double) xres / (double) yres);
			double bestdiff = 10000000000;

			// just loop through all the stupid modes.
			for(uint32_t m = 0; m < gop->Mode->MaxMode; m++)
			{
				efi_graphics_output_mode_information* modeinfo = 0;
				size_t sz = sizeof(efi_graphics_output_mode_information);

				stat = gop->QueryMode(gop, m, &sz, &modeinfo);
				efi::abort_if_error(stat, "failed to query modeinfo for mode %u", m);

				// make sure we only accept sane video modes!
				if(modeinfo->PixelFormat != PixelRedGreenBlueReserved8BitPerColor)
				{
					auto h = modeinfo->HorizontalResolution;
					auto v = modeinfo->VerticalResolution;

					// efi::println("mode %u: %d x %d (%f)", m, h, v, (double) h / (double) v);

					double score = (h + v) / ((double) h / (double) v);
					if(double d = __abs(score - wanted); d < bestdiff)
					{
						bestdiff = d;
						bestMode = m;
						bestmodeinfo = modeinfo;
					}
				}
			}
		}

		if(bestMode == -1) efi::abort("no valid video mode!");

		{
			pixelsPerScanline = bestmodeinfo->PixelsPerScanLine;
			xResolution = bestmodeinfo->HorizontalResolution;
			yResolution = bestmodeinfo->VerticalResolution;

			efi::println("setting mode %u: %ux%u (scan line %u)", bestMode, xResolution, yResolution, pixelsPerScanline);


			// dewit
			auto stat = gop->SetMode(gop, bestMode);
			efi::abort_if_error(stat, "failed to set video mode!");

			// i think we can still print stuff right? efi should be smarter than this
			efi::systable()->ConOut->ClearScreen(efi::systable()->ConOut);
			efi::println("video mode set!\n");
		}
	}
}
}



































