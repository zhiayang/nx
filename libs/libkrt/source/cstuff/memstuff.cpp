// memstuff.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stddef.h>

#include "stdint.h"

extern "C" void* memccpy(void* dest_ptr, const void* src_ptr, int c, size_t n)
{
	unsigned char* dest = (unsigned char*) dest_ptr;
	const unsigned char* src = (const unsigned char*) src_ptr;
	for(size_t i = 0; i < n; i++)
	{
		if((dest[i] = src[i]) == (unsigned char) c)
			return dest + i + 1;
	}
	return NULL;
}

extern "C" void* memchr(const void* s, int c, size_t size)
{
	const unsigned char* buf = (const unsigned char*) s;
	for(size_t i = 0; i < size; i++)
		if(buf[i] == c)
			return (void*) (buf + i);
	return NULL;
}

extern "C" int memcmp(const void* a_ptr, const void* b_ptr, size_t size)
{
	const unsigned char* a = (const unsigned char*) a_ptr;
	const unsigned char* b = (const unsigned char*) b_ptr;
	for(size_t i = 0; i < size; i++)
	{
		if(a[i] < b[i])
			return -1;
		if(a[i] > b[i])
			return +1;
	}
	return 0;
}

inline static void* memcpy_slow(void* __restrict__ dstptr, const void* __restrict__ srcptr, size_t length)
{
	uint8_t* __restrict__ dst = (uint8_t* __restrict__) dstptr;
	const uint8_t* __restrict__ src = (const uint8_t* __restrict__) srcptr;
	for(size_t i = 0; i < length; i += sizeof(uint8_t))
		dst[i] = src[i];

	return dstptr;
}

extern "C" void* memmove(void* dest_ptr, const void* src_ptr, size_t n)
{
	unsigned char* dest = (unsigned char*) dest_ptr;
	const unsigned char* src = (const unsigned char*) src_ptr;
	if(dest < src)
		for(size_t i = 0; i < n; i++)
			dest[i] = src[i];
	else
		for(size_t i = n; i != 0; i--)
			dest[i - 1] = src[i - 1];

	return dest_ptr;
}


extern "C" void* memrchr(const void* ptr, int c, size_t n)
{
	const unsigned char* buf = (const unsigned char*) ptr;
	for(size_t i = n; i != 0; i--)
		if(buf[i-1] == c)
				return (void*) (buf + i-1);
	return NULL;
}

extern "C" void* memset(void* ptr, int value, size_t num)
{
	#if 1
	// 'stosl' will use the value in eax but we only want the value in al
	// so we make eax = al << 24 | al << 16 | al << 8 | al
	if((value & 0xff) == 0)
		// this is a little optimization because memset is most often called with value = 0
		value = 0;

	else
	{
		value = (value & 0xff) | ((value & 0xff) << 8);
		value = (value & 0xffff) | ((value & 0xffff) << 16);
	}

	void* temporaryPtr = ptr;
	asm volatile("rep stosl ; mov %3, %2 ; rep stosb" : "+D"(temporaryPtr) : "a"(value), "c"(num / 4), "r"(num % 4) : "cc", "memory");
	return ptr;
	#else

	for(size_t i = 0; i < num; i++)
		((char*) ptr)[i] = (char) value;

	return ptr;

	#endif
}

extern "C" void* memcpy(void* __restrict__ dstptr, const void* __restrict__ srcptr, size_t length)
{
	const unsigned long unalignmask = sizeof(unsigned long) - 1;
	const unsigned long srcunalign = (unsigned long) srcptr & unalignmask;
	const unsigned long dstunalign = (unsigned long) dstptr & unalignmask;

	if(srcunalign != dstunalign)
		return memcpy_slow(dstptr, srcptr, length);

	union
	{
		unsigned long srcval;
		const uint8_t* __restrict__ src8;
		const uint16_t* __restrict__ src16;
		const uint32_t* __restrict__ src32;
		const uint64_t* __restrict__ src64;
		const unsigned long* __restrict__ srcul;
	};
	srcval = (unsigned long) srcptr;

	union
	{
		unsigned long dstval;
		uint8_t* __restrict__ dst8;
		uint16_t* __restrict__ dst16;
		uint32_t* __restrict__ dst32;
		uint64_t* __restrict__ dst64;
		unsigned long* __restrict__ dstul;
	};
	dstval = (unsigned long) dstptr;

	if(dstunalign)
	{
		if(1 <= length && !(dstval & (1 - 1)) && (dstval & (2 - 1)))
			*dst8++ = *src8++,
			length -= 1;

		if(2 <= length && !(dstval & (2 - 1)) && (dstval & (4 - 1)))
			*dst16++ = *src16++,
			length -= 2;

		if(4 <= length && !(dstval & (4 - 1)) && (dstval & (8 - 1)))
			*dst32++ = *src32++,
			length -= 4;
	}

	size_t numcopies = length / sizeof(unsigned long);

	for(size_t i = 0; i < numcopies; i++)
		*dstul++ = *srcul++;

	length -= numcopies * sizeof(unsigned long);

	if(length)
	{
		if(4 <= length)
			*dst32++ = *src32++,
			length -= 4;

		if(2 <= length)
			*dst16++ = *src16++,
			length -= 2;

		if(1 <= length)
			*dst8++ = *src8++,
			length -= 1;
	}

	return dstptr;
}
