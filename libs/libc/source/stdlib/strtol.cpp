// strtol.cpp
// Copyright (c) 2011 - 2014, Jonas 'Sortie' Termansen
// Licensed under the GNU LGPL.

#include "../../include/stdlib.h"
#include "../../include/stdint.h"
#include "../../include/ctype.h"
#include "../../include/limits.h"
#include "../../include/assert.h"


#ifndef STRTOL
#define STRTOL strtol
#define STRTOL_CHAR char
#define STRTOL_L(x) x
#define STRTOL_ISSPACE isspace
#define STRTOL_INT long
#define STRTOL_UNSIGNED_INT unsigned long
#define STRTOL_INT_MIN LONG_MIN
#define STRTOL_INT_MAX LONG_MAX
#define STRTOL_INT_IS_UNSIGNED false
#endif

// Convert a character into a digit.
static int debase(STRTOL_CHAR c)
{
	if(STRTOL_L('0') <= c && c <= STRTOL_L('9'))
		return c - STRTOL_L('0');
	if(STRTOL_L('a') <= c && c <= STRTOL_L('z'))
		return 10 + c - STRTOL_L('a');
	if(STRTOL_L('A') <= c && c <= STRTOL_L('Z'))
		return 10 + c - STRTOL_L('A');
	return -1;
}

// Determine whether a multiplication of two integers would overflow/underflow.
// This is easy if we have a larger integer type, otherwise we'll be creative.
// template <class T_INT, bool T_INT_IS_UNSIGNED, class T_UNSIGNED_INT, T_INT T_INT_MIN, T_INT T_INT_MAX>
// static bool would_multiplication_overflow(T_INT a, T_INT b)
// {
// 	// Prevent accidental divisons by zero in this simple case.
// 	if(!a || !b)
// 		return false;

// 	// Check if we have a 64-bit integer that it is large enough.
// 	if(sizeof(T_INT)*2 <= sizeof(int64_t))
// 	{
// 		int64_t ret_large = (int64_t) a * (int64_t) b;
// 		return ret_large < (int64_t) T_INT_MIN ||
// 		       (int64_t) T_INT_MAX < ret_large;
// 	}

// 	// Check if we have a 128-bit integer that it is large enough.

// 	if(sizeof(T_INT)*2 <= sizeof(__int128))
// 	{
// 		__int128 ret_large = (__int128) a * (__int128) b;
// 		return ret_large < (__int128) T_INT_MIN ||
// 		       (__int128) T_INT_MAX < ret_large;
// 	}


// 	// The fallback strategy is to determine the largest b given a that will not
// 	// overflow and then see if b is within range. This is trivial if in the
// 	// unsigned integer case.
// 	if(T_INT_IS_UNSIGNED)
// 	{
// 		T_INT max_b = a / T_INT_MAX;
// 		return max_b < b;
// 	}

// 	// We have to deal with some cases for signed integers. We'll assume signed
// 	// integers are in two's complement and use - (unsigned int) value to take
// 	// the absolute value of a negative value as an unsigned integer in a manner
// 	// that is defined in C. Whether we use the smallest or largest value depend
// 	// on whether the sign of a and b is identical.
// 	else
// 	{
// 		T_UNSIGNED_INT a_abs = a < 0 ? - (T_UNSIGNED_INT) a : a;
// 		T_UNSIGNED_INT b_abs = b < 0 ? - (T_UNSIGNED_INT) b : b;
// 		T_UNSIGNED_INT min_abs = - (T_UNSIGNED_INT) T_INT_MIN;
// 		T_UNSIGNED_INT max_abs = T_INT_MAX;
// 		T_UNSIGNED_INT limit_pos = (0 <= a && 0 <= b) ||(a < 0 && b < 0);
// 		T_UNSIGNED_INT limit = limit_pos ? max_abs : min_abs;
// 		T_UNSIGNED_INT max_b = a_abs / limit;
// 		return max_b < b_abs;
// 	}
// }

extern "C" STRTOL_INT STRTOL(const STRTOL_CHAR* str, STRTOL_CHAR** endptr, int base)
{
	const STRTOL_CHAR* origstr = str;
	int origbase = base;

	// Skip any leading white space.
	while(STRTOL_ISSPACE(*str))
		str++;

	// Reject bad bases.
	if(base < 0 || 36 < base)
	{
		if(endptr)
			*endptr = (STRTOL_CHAR*) str;
		// return errno = EINVAL, 0;
		return 0;
	}

	bool negative = false;
	STRTOL_CHAR c = *str;

	// Handle a leading sign character.
	if(c == STRTOL_L('-'))
		str++, negative = true;
	else if(c == STRTOL_L('+'))
		str++, negative = false;

	// Autodetect base 8 or base 16.
	if(!base && str[0] == STRTOL_L('0'))
	{
		if((str[1] == STRTOL_L('x') || str[1] == STRTOL_L('X')) &&
		     (str[2] && debase(str[2]) < 16))
			str += 2, base = 16;
		else if(0 <= debase(str[1]))
			str++, base = 8;
	}

	// Default to base 10.
	if(!base)
		base = 10;

	// Skip the leading '0x' prefix in base 16 for hexadecimal integers.
	if(origbase == 16 &&
	     str[0] == STRTOL_L('0') &&
	     (str[1] == STRTOL_L('x') || str[1] == STRTOL_L('X')))
		str += 2;

	// Determine what value will be returned on overflow/underflow.
	STRTOL_INT overflow_value = negative && !STRTOL_INT_IS_UNSIGNED ?
	                            STRTOL_INT_MIN :
	                            STRTOL_INT_MAX;

	// Convert a single character at a time.
	STRTOL_INT result = 0;
	size_t numconvertedchars = 0;
	bool overflow_occured = false;
	while((c = *str))
	{
		// Stop if we encounterd a character that doesn't fit in this base.
		int val = debase(c);
		if(val < 0 || base <= val)
			break;

		// Attempt to multiply the accumulator with the current base.
		// if(would_multiplication_overflow<STRTOL_INT,
		//                                    STRTOL_INT_IS_UNSIGNED,
		//                                    STRTOL_UNSIGNED_INT,
		//                                    STRTOL_INT_MIN,
		//                                    STRTOL_INT_MAX>
		//                                   (result, (STRTOL_INT) base))
		// 	overflow_occured = true, result = overflow_value;
		// else
		{
			STRTOL_INT new_result = result * (STRTOL_INT) base;
			assert( negative || result <= new_result);
			assert(!negative || result >= new_result);
			result = new_result;
		}

		// Attempt to add the latest digit to the accumulator (positive).
		if((STRTOL_INT_IS_UNSIGNED || !negative) &&
		     (STRTOL_INT) val <= (STRTOL_INT) (STRTOL_INT_MAX - result))
			result += (STRTOL_INT) val;

		// Attempt to subtract the latest digit to the accumulator (negative).
		else if((!STRTOL_INT_IS_UNSIGNED && negative) &&
		          (STRTOL_UNSIGNED_INT) val < ((STRTOL_UNSIGNED_INT) result - (STRTOL_UNSIGNED_INT) STRTOL_INT_MIN))
			result -= (STRTOL_INT) val;

		// Handle the case where the addition/subtract would overflow/underflow.
		else
			overflow_occured = true, result = overflow_value;

		str++;
		numconvertedchars++;
	}

	// If no characters were successfully converted, rewind to the start, also
	// rewinding past skipped whitespace and sign characters and such.
	if(!numconvertedchars)
		str = origstr, result = 0;

	// Let the caller know where we got to.
	if(endptr)
		*endptr = (STRTOL_CHAR*) str;

	// Handle the special case where we are creating an unsigned integer and the
	// string was negative and non-zero and no overflow occured, then we treat
	// it as (the maximum value+1) minus (the negative string as integer).
	if(STRTOL_INT_IS_UNSIGNED && negative && result && !overflow_occured)
		result = STRTOL_INT_MAX - (result-1);

	return result;
}
