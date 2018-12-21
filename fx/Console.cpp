// Console.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <stdint.h>
#include <stdarg.h>
#include "FxLoader.hpp"

namespace Console
{
	uint8_t CursorX = 0, CursorY = 0;
	const uint32_t Space = ((uint16_t)(' ') | (0x0F << 8) | (((uint32_t)(uint16_t)(' ') | (0x0F << 8)) << 16));
	const uint8_t TabWidth = 4;


	#define Port		0x3F8		// Serial port 1

	void WriteCharToSerial(uint8_t Ch)
	{
		while(!(IOPort::ReadByte(Port + 5) & 0x20));
		IOPort::WriteByte(Port, Ch);
	}


	void Initialise()
	{
		// Hide the stupid cursor
		IOPort::Write16(0x3D4, 0x200A);
		IOPort::Write16(0x3D4, 0xB);

		ClearScreen(0x00);


		IOPort::WriteByte(Port + 1, 0x00);    // Disable all interrupts
		IOPort::WriteByte(Port + 3, 0x80);    // Enable DLAB (set baud rate divisor)
		IOPort::WriteByte(Port + 0, 0x01);    // Set divisor to 1 (lo byte) max baud
		IOPort::WriteByte(Port + 1, 0x00);    //			(hi byte)
		IOPort::WriteByte(Port + 3, 0x03);    // 8 bits, no parity, one stop bit
		IOPort::WriteByte(Port + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
		IOPort::WriteByte(Port + 4, 0x0B);    // IRQs enabled, RTS/DSR set
	}

	static void* memset32(void* dst, uint32_t val, uint64_t len)
	{
		uintptr_t d0 = 0;
		uint64_t uval = ((uint64_t) val << 32) + val;
		asm volatile(
			"rep stosq"
			:"=&D" (d0), "+&c" (len)
			:"0" (dst), "a" (uval)
			:"memory");

		return dst;
	}

	void ClearScreen(uint8_t Colour)
	{
		uint32_t s = (uint16_t)((' ') | (Colour << 8));

		for(int i = 0; i < 25; i++)
			memset32((uint16_t*)0xB8000 + i * 80, s, 80);

		CursorX = 0;
		CursorY = 0;
	}

	static void PrintChar(uint8_t Char, uint32_t Colour)
	{
		WriteCharToSerial(Char);
		uint16_t* Location;

		if(Char == '\b')
		{
			if(CursorX != 0)
			{
				CursorX--;
				PrintChar(' ', Colour);
				CursorX--;
			}
			else if(CursorY > 0)
			{
				CursorX = 79;
				PrintChar(' ', Colour);
				CursorX = 79;
			}
		}

		else if(Char == '\t')
		{
			CursorX = (CursorX + TabWidth) & ~(TabWidth - 1);
		}

		else if(Char == '\r')
		{
			CursorX = 0;
		}

		else if(Char == '\n')
		{
			CursorX = 0;
			CursorY++;
		}

		else if(Char >= ' ')
		{
			Location = (uint16_t*)0xB8000 + (CursorY * 80 + CursorX);
			*Location = (uint16_t)(Char | (Colour << 8));
			CursorX++;
		}

		if(CursorX >= 80)
		{
			CursorX = 0;
			CursorY++;
		}

		ScrollUp();
	}


	static void PrintChar(uint8_t Char)
	{
		PrintChar(Char, 0x0F);
	}

	void ScrollUp()
	{
		if(CursorY >= 25)
		{
			memcpy((uint16_t*) 0xB8000, (uint16_t*) 0xB8000 + (CursorY - 24) * 80, (49 - CursorY) * 80 * 2);
			memset32((uint16_t*) 0xB8000 + (49 - CursorY) * 80, Space, 40);
			CursorY = 24;
		}
	}









	uint64_t PrintString(const char* string, int64_t length)
	{
		uint64_t ret = 0;
		for(uint64_t i = 0; i < (length < 0 ? strlen(string) : (uint64_t) length); i++)
		{
			PrintChar((uint8_t) string[i]);
			ret++;
		}

		return ret;
	}


	static uint64_t PrintHex_NoPrefix(uint64_t n)
	{
		int64_t i = 0;
		uint64_t ret = 0;


		// Mask bits of variables to determine size, and therefore how many digits to print.

		if((n & 0xF000000000000000) == 0)
			if((n & 0xFF00000000000000) == 0)
				if((n & 0xFFF0000000000000) == 0)
					if((n & 0xFFFF000000000000) == 0)
						if((n & 0xFFFFF00000000000) == 0)
							if((n & 0xFFFFFF0000000000) == 0)
								if((n & 0xFFFFFFF000000000) == 0)
									if((n & 0xFFFFFFFF00000000) == 0)
										if((n & 0xFFFFFFFFF0000000) == 0)
											if((n & 0xFFFFFFFFFF000000) == 0)
												if((n & 0xFFFFFFFFFFF00000) == 0)
													if((n & 0xFFFFFFFFFFFF0000) == 0)
														if((n & 0xFFFFFFFFFFFFF000) == 0)
															if((n & 0xFFFFFFFFFFFFFF00) == 0)
																if((n & 0xFFFFFFFFFFFFFFF0) == 0)
																	i = 0;
																else
																	i = 4;
															else
																i = 8;
														else
															i = 12;
													else
														i = 16;
												else
													i = 20;
											else
												i = 24;
										else
											i = 28;
									else
										i = 32;
								else
									i = 36;
							else
								i = 40;
						else
							i = 44;
					else
						i = 48;
				else
					i = 52;
			else
				i = 56;
		else
			i = 60;


		for(; i >= 0; i -= 4)
		{
			int64_t tmp = (n >> i) & 0xF;

			if(tmp >= 0xA)
				PrintChar((uint8_t) (tmp - 0xA + 'A'));

			else
				PrintChar((uint8_t) (tmp + '0'));

			ret++;
		}

		return ret;
	}

	static uint64_t PrintHex_Precision_NoPrefix(uint64_t n, int8_t leadingzeroes, bool padzeroes, int8_t prec, bool ppf)
	{
		(void) padzeroes;
		if(prec < 0)
		{
			if(ppf)
				PrintString("0x", -1);

			return PrintHex_NoPrefix(n) + 2;
		}


		int64_t i = (prec * 4) - 4;
		uint64_t ret = 0;

		if(ppf)
			PrintString("0x", -1);

		if(n == 0)
		{
			for(int8_t d = 0; d < leadingzeroes; d++)
				PrintString("0", -1);

			return (uint64_t) leadingzeroes;
		}



		for(; i >= 0; i -= 4)
		{
			int64_t tmp = (n >> i) & 0xF;


			if(tmp >= 0xA)
				PrintChar((uint8_t) (tmp - 0xA + 'A'));

			else
				PrintChar((uint8_t) (tmp + '0'));

			ret++;
		}

		return ret;
	}


	#define abs(x)			((x) < 0 ? (-(x)) : (x))


	static uint64_t PrintInteger_Signed(int64_t num, int8_t Width = -1)
	{
		uint64_t ret = 0;

		if(num == 0)
		{
			if(Width != -1)
			{
				for(int g = 0; g < Width; g++)
				{
					PrintChar('0');
					ret++;
				}
			}
			else
			{
				PrintChar('0');
				ret++;
			}

			return ret;
		}

		if(num < 0){ PrintChar('-'); ret++; }
		if(Width != -1)
		{
			uint64_t n = (uint64_t) abs(num);
			uint8_t k = 0;
			while(n > 0)
			{
				n /= 10;
				k++;
			}

			while(Width > k)
			{
				PrintChar('0');
				k++;
				ret++;
			}
		}

		char out[12] = { 0 };
		char* k = Util::ConvertToString(num, out);
		auto r = PrintString(k, -1) + ret;

		return r;
	}


	void Print(const char* str, ...)
	{
		va_list args;
		va_start(args, str);
		Print(str, args);
		va_end(args);
	}

	void Print(const char* string, va_list args)
	{
		// Note:
		/*
			This is totally non-standard printf.
			A number of custom formatters are available:
			%r resets to white. (no arguments)

			The behaviour of hex printing is quite different:
			%x prints in uppercase.
			%X prints in lowercase.

			%x prints /with/ the leading '0x'
			%#x prints without.

			%[width]d now uses spaces for padding by default, while %0[width]d uses zeroes, as per printf-standard.

			We now 'almost' follow the printf standard of  %[parameter][flags][width][.precision][length]type
			/almost/.

			'Parameter' is not supported. It might be, when a valid use case is encountered.
			'Flags' in this case is either '0', '#', '+', '^' (custom) or ' '.
			'Width' is fully supported.
			'Precision' is also fully supported.

			'Length' is only partially supported -- they will only be taken into account for floating point numbers,
			because for other types we accept the largest type by default.
			Additionally, we always accept at least a 'double' for those -- therefore 'L' and 'l' function the same way,
			in that we instead look for a long double.



			However, because front-padding (align right) for hex numbers is handled in the PrintHex_Precision_ functions,
			the Precision formatter is used to control how many zeroes to print when the input number is zero.

			For example: %16x will print [width] zeroes by default if the input is zero.
			However, %16.3x will print up to 3 zeroes, padding the rest with spaces.
		*/


		bool IsFormat = false;
		bool DisplaySign = false;
		bool OmitZeroX = false;
		bool IsParsingPrecision = false;
		bool LeftAlign = false;
		bool PadZeroes = false;
		bool PadSignedSpace = false;

		// we don't support ints smaller than 64bit.
		// 1 is long double.
		int8_t Precision = -1;
		int8_t Width = -1;

		uint64_t PrintedChars = 0;

		int64_t z = 0;
		uint64_t x = 0;
		char* s = 0;
		char ch = 0;

		uint64_t length = strlen(string);

		char widthbuf[8] = { 0 };
		char precsbuf[8] = { 0 };

		// %[parameter][flags][width][.precision][length]type

		for(uint64_t i = 0; i < length; i++)
		{
			char c = string[i];

			if(!IsFormat && c == '%')
			{
				IsFormat = true;
				continue;
			}
			if(IsFormat)
			{
				switch(c)
				{
					case '%':
						PrintChar('%');
						break;

					// Standard, parameter types

					case 'd':
					case 'i':
					case 'u':
						z = va_arg(args, int64_t);
						if(DisplaySign)
						{
							if(z > 0)
								PrintChar('+');

							DisplaySign = false;
						}

						if(PadSignedSpace)
						{
							PadSignedSpace = false;
							if(z < 0)
								PrintChar(' ');
						}

						PrintedChars = PrintInteger_Signed(z, Width);

						PadZeroes = false;
						break;

					case 's':
						s = va_arg(args, char*);

						// check to pad with spaces.
						if(Width > 0)
						{
							int64_t wd = Width;
							for(int64_t m = 0; m < wd; m++)
								PrintChar(' ');
						}

						PrintString(s, Precision);
						break;


					case 'X':
					case 'x':
					case 'p':
						x = va_arg(args, uint64_t);

						if(c == 'p')
							Precision = 16;

						// now handle the padding rubbish.
						// check if we need to align left.
						if(LeftAlign && Width > 0)
						{
							PrintedChars = PrintHex_Precision_NoPrefix(x, -1, PadZeroes, Precision, !OmitZeroX);

							LeftAlign = false;
							for(uint64_t tps = 0; tps < (uint8_t) Width - PrintedChars; tps++)
							{
								PrintChar(' ');
							}
						}
						else
						{
							// handle cases where we use width instead.
							if(Width > 0)
								Precision = Width;

							PrintHex_Precision_NoPrefix(x, Width > 0 ? Width : 1, PadZeroes, Precision, !OmitZeroX);
						}

						PadZeroes = false;
						OmitZeroX = false;
						break;

					case 'c':
						ch = (char) va_arg(args, int);
						PrintChar((uint8_t) ch);
						break;


					// Flags
					case '+':
						DisplaySign = true;
						IsFormat = true;
						continue;

					// Note this is the reverse of standard behaviour; we'll print '0x' every time unless # is specified.
					case '#':
						OmitZeroX = true;
						IsFormat = true;
						continue;

					case '^':
						IsFormat = true;
						continue;

					case '-':
						LeftAlign = true;
						IsFormat = true;
						continue;

					case ' ':
						PadSignedSpace = true;
						IsFormat = true;
						continue;


					// Width/precision
					case '.':
						IsParsingPrecision = true;
						IsFormat = true;
						continue;


					default:
						if(IsParsingPrecision)
						{
							int z1 = 0;
							uint64_t f1 = i;
							for(z1 = 0, f1 = i; ((string[f1] >= '0') && (string[f1] <= '9')) || string[f1] == '*'; z1++, f1++)
							{
								precsbuf[z1] = string[f1];
							}
							if(precsbuf[0] == '*')
								Precision = (int8_t) va_arg(args, uint64_t);

							else
								Precision = (int8_t) Util::ConvertToInt(precsbuf, 10);

							// -1 because continue; increments i
							i = f1 - 1;
							IsFormat = true;

							memset(precsbuf, 0, 8);
							IsParsingPrecision = false;

							continue;
						}
						else
						{
							int z1 = 0;
							uint64_t f1 = i;
							for(z1 = 0, f1 = i; ((string[f1] >= '0') && (string[f1] <= '9')) || string[f1] == '*'; z1++, f1++)
							{
								widthbuf[z1] = string[f1];
							}

							if(widthbuf[0] == '0')
								PadZeroes = true;

							if(widthbuf[0] == '*')
								Width = (int8_t) va_arg(args, uint64_t);

							else
								Width = (int8_t) Util::ConvertToInt(widthbuf, 10);



							// -1 because continue; increments i
							i = f1 - 1;
							IsFormat = true;

							memset(widthbuf, 0, 8);
							continue;
						}
				}
				IsFormat = false;
				DisplaySign = false;

				Precision = -1;
				Width = -1;
				IsParsingPrecision = false;
			}
			else
			{
				PrintChar((uint8_t) c);
			}
		}
	}
}







