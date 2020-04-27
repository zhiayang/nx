// ps2/keyboard.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdio.h>
#include <assert.h>

#include "keyboard.h"
#include "translation.h"

namespace ps2
{
	// maybe see here also: https://gist.github.com/MightyPork/6da26e382a7ad91b5496ee55fdc73db2

	bool Keyboard::addByte(uint8_t x)
	{
		this->buffer.write(x);

		// if it wasn't something strange, decode it!
		if(x != 0xE0 && x != 0xE1 && x != 0xF0)
		{
			auto hid = this->decodeHID();

			// temporary!!!!!!!
			return this->print(hid);
		}

		return false;
	}

	uint8_t Keyboard::decodeHID()
	{
		// start the thing.
		auto x = this->buffer.read();

		bool e0 = false;
		bool f0 = false;


		if(x == 0xE0)
		{
			e0 = true;
			x = this->buffer.read();

			if(x == 0xF0)
			{
				f0 = true;
				x = this->buffer.read();
			}
		}
		else if(x == 0xF0)
		{
			f0 = true;
			x = this->buffer.read();
		}

		auto is_shift = [](uint8_t x) -> bool {
			return x == 0x59 || x == 0x12;
		};

		// this is temporary!!!!
		if(!f0 && !e0 && is_shift(x))
			this->shifting = true;

		else if(f0 && !e0 && is_shift(x))
			this->shifting = false;

		else if(!f0 && !e0)
		{
			return tables::ScanCode2_US_PS2_HID[x];
		}
		else if(!f0 && e0)
		{
			return tables::ScanCode2_US_E0_HID[x];
		}

		return 0;
	}


	bool Keyboard::print(uint8_t hid)
	{
		// the hid table is a little strange. it's like 5 years old, forgive me.
		size_t idx = hid + 3;
		idx *= 4;
		idx += 1;

		if(this->shifting)
			idx += 1;

		assert(idx < 4 * 0xFF);
		char x = tables::USB_HID[idx];


		// temporary as fuck!
		if(x != 0)
		{
			putchar(x);
			return true;
		}

		return false;
	}
}











