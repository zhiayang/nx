// memcpy.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <stddef.h>
#include "../../include/stdint.h"

inline static void* memcpy_slow(void* __restrict__ dstptr, const void* __restrict__ srcptr, size_t length)
{
	uint8_t* __restrict__ dst = (uint8_t* __restrict__) dstptr;
	const uint8_t* __restrict__ src = (const uint8_t* __restrict__) srcptr;
	for(size_t i = 0; i < length; i += sizeof(uint8_t))
		dst[i] = src[i];

	return dstptr;
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
