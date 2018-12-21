// Utilities.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "FxLoader.hpp"

extern "C" void* memset(void* ptr, int value, uint64_t num)
{
	// 'stosl' will use the value in eax but we only want the value in al
	// so we make eax = al << 24 | al << 16 | al << 8 | al
	if((value & 0xFF) == 0)
		// this is a little optimization because memset is most often called with value = 0
		value = 0;

	else
	{
		value = (value & 0xFF) | ((value & 0xFF) << 8);
		value = (value & 0xFFFF) | ((value & 0xFFFF) << 16);
	}

	void* temporaryPtr = ptr;
	asm volatile("rep stosl ; mov %3, %2 ; rep stosb" : "+D"(temporaryPtr) : "a"(value), "c"(num / 4), "r"(num % 4) : "cc", "memory");
	return ptr;
}

extern "C" void* memcpy(void* dest, const void* src, uint64_t len)
{

	asm volatile("cld; rep movsb"
	: "+c" (len), "+S" (src), "+D" (dest)
	:: "memory");


	return dest;
}

extern "C" void* memmove(void* dst, const void* src, uint64_t n)
{
	uint8_t* a = (uint8_t*) dst;
	const uint8_t* b = (uint8_t*) src;
	if(a <= b || b >= (a + n))
	{
		// No overlap, use memcpy logic (copy forward)
		memcpy(dst, src, n);
	}
	else
	{
		asm volatile("std");
		memcpy(dst, src, n);
		asm volatile("cld");
	}
	return dst;
}

extern "C" size_t strlen(const char* str)
{
	size_t len = 0;
	const char* endPtr = str;
	asm("repne scasb" : "+D"(endPtr) : "a"(0), "c"(~0) : "cc");
	len = ((uintptr_t) endPtr - (uintptr_t) str) - 1;
	return len;
}

extern "C" void __assert(const char* filename, unsigned long line, const char* function_name, const char* expression)
{
	Console::Print("assert(%s) failed!\n", expression);
	Console::Print("(%s:%d), function %s\n", filename, line, function_name);
	Console::Print("[fx] has encountered a critical error and cannot continue further\n");
	asm volatile("cli; hlt");
}



namespace Util
{
	void DumpBytes(uint64_t address, uint64_t length)
	{
		for(uint64_t i = 0; i < length; i++)
		{
			Console::Print("%#02X ", *((uint8_t*)(address + i)));
			if((i + 1) % 16 == 0)
				Console::Print("\n");
		}
	}

	char* ConvertToString(int64_t num, char* out)
	{
		char* dest = out;
		int result = 0;
		int64_t copy = num;

		if(num < 0)
		{
			*dest++ = '-';
			result++;

			int offset = 0;
			while(copy < -9)
			{
				copy /= 10;
				offset++;
			}
			result += offset;

			while(offset >= 0)
			{
				dest[offset] = '0' - num % 10;
				num /= 10;
				offset--;
			}
		}
		else
		{
			int offset = 0;
			while(copy > 9)
			{
				copy /= 10;
				offset++;
			}
			result += offset;
			while(offset >= 0)
			{
				dest[offset] = '0' + num % 10;
				num /= 10;
				offset--;
			}
		}
		return dest;
	}

	static int Debase(char c)
	{
		if ( '0' <= c && c <= '9' ) { return c - '0'; }
		if ( 'a' <= c && c <= 'z' ) { return 10 + c - 'a'; }
		if ( 'A' <= c && c <= 'Z' ) { return 10 + c - 'A'; }
		return -1;
	}

	int64_t ParseInteger(const char* str, char** endptr, int base)
	{
		const char* origstr = str;
		int origbase = base;

		while(*str == ' ' || *str == '\t')
		{
			str++;
		}

		if(base < 0 || base > 36)
		{
			if(endptr)
			{
				*endptr = (char*) str;
			}
			return 0;
		}

		int64_t result = 0;
		bool negative = false;
		char c = *str;
		if(c == '-')
		{
			str++;
			negative = true;
		}
		if(c == '+')
		{
			str++;
			negative = false;
		}
		if(!base && str[0] == '0')
		{
			if(str[1] == 'x' || str[1] == 'X')
			{
				str += 2;
				base = 16;
			}

			else if(0 <= Debase(str[1]) && Debase(str[1]) < 8)
			{
				str++;
				base = 8;
			}
		}
		if(!base)
		{
			base = 10;
		}
		if(origbase == 16 && str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
		{
			str += 2;
		}

		uint64_t numconvertedchars = 0;
		while((c = *str))
		{
			int val = Debase(c);
			if(val < 0)
				break;

			if(base <= val)
				break;
			if(negative)
				val = -val;

			// TODO: Detect overflow!
			result = result * (int64_t) base + (int64_t) val;
			str++;
			numconvertedchars++;
		}
		if(!numconvertedchars)
		{
			str = origstr;
			result = 0;
		}
		if(endptr)
		{
			*endptr = (char*) str;
		}

		return result;
	}

	int64_t ConvertToInt(char* str, uint8_t base)
	{
		return ParseInteger(str, 0, base);
	}
}










