/*******************************************************************************

    Copyright(C) Jonas 'Sortie' Termansen 2011, 2013.

    This file is part of the Sortix C Library.

    The Sortix C Library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    The Sortix C Library is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
    License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with the Sortix C Library. If not, see <http://www.gnu.org/licenses/>.

    __/stdint.h
    Integer types.

*******************************************************************************/

#ifndef INCLUDE____STDINT_H
#define INCLUDE____STDINT_H

#include <sys/cdefs.h>
#include <bits/wordsize.h>

__BEGIN_DECLS

/* TODO: Make the compiler provide all this information using __ prefix. */
/* TODO: Use __XINTXX_TYPE__ constants when switching to gcc 4.7.x or newer. */

/* Signed constants. */
#define __INT8_C(c) c
#define __INT16_C(c) c
#define __INT32_C(c) c
#if __WORDSIZE == 64
#define __INT64_C(c) c ## L
#else
#define __INT64_C(c) c ## LL
#endif

/* Unsigned constants. */
#define __UINT8_C(c) c
#define __UINT16_C(c) c
#define __UINT32_C(c) c ## U
#if __WORDSIZE == 64
#define __UINT64_C(c) c ## UL
#else
#define __UINT64_C(c) c ## ULL
#endif

/* Maxmimal constants. */
#if __WORDSIZE == 64
#define __INTMAX_C(c) c ## L
#define __UINTMAX_C(c) c ## UL
#else
#define __INTMAX_C(c) c ## LL
#define __UINTMAX_C(c) c ## ULL
#endif

/* Define basic signed types. */
typedef signed char __int8_t;
typedef short int __int16_t;
typedef int __int32_t;
#if __WORDSIZE == 64
typedef long int __int64_t;
#else
typedef long long int __int64_t;
#endif

#define __INT8_MIN (-128)
#define __INT16_MIN (-32767-1)
#define __INT32_MIN (-2147483647-1)
#define __INT64_MIN (-__INT64_C(9223372036854775807)-1)

#define __INT8_MAX (127)
#define __INT16_MAX (32767)
#define __INT32_MAX (2147483647)
#define __INT64_MAX (__INT64_C(9223372036854775807))

#define __PRId8 "d"
#define __PRIi8 "i"
#define __SCNd8 "hhd"
#define __SCNi8 "hhi"
#define __PRId16 "d"
#define __PRIi16 "i"
#define __SCNd16 "hd"
#define __SCNi16 "hi"
#define __PRId32 "d"
#define __PRIi32 "i"
#define __SCNd32 "d"
#define __SCNi32 "i"
#if __WORDSIZE == 64
#define __PRId64 "ld"
#define __PRIi64 "li"
#define __SCNd64 "ld"
#define __SCNi64 "li"
#else
#define __PRId64 "lld"
#define __PRIi64 "lli"
#define __SCNd64 "lld"
#define __SCNi64 "lli"
#endif

/* Define basic unsigned types. */
typedef unsigned char __uint8_t;
typedef unsigned short int __uint16_t;
typedef unsigned int __uint32_t;
#if __WORDSIZE == 64
typedef unsigned long int __uint64_t;
#else
typedef unsigned long long int __uint64_t;
#endif

#define __UINT8_MAX (255)
#define __UINT16_MAX (65535)
#define __UINT32_MAX (4294967295U)
#define __UINT64_MAX (__UINT64_C(18446744073709551615))

#define __PRIo8 "o"
#define __PRIu8 "u"
#define __PRIx8 "x"
#define __PRIX8 "X"
#define __SCNo8 "hho"
#define __SCNu8 "hhu"
#define __SCNx8 "hhx"
#define __PRIo16 "o"
#define __PRIu16 "u"
#define __PRIx16 "x"
#define __PRIX16 "X"
#define __SCNo16 "ho"
#define __SCNu16 "hu"
#define __SCNx16 "hx"
#define __PRIo32 "o"
#define __PRIu32 "u"
#define __PRIx32 "x"
#define __PRIX32 "X"
#define __SCNo32 "o"
#define __SCNu32 "u"
#define __SCNx32 "x"
#if __WORDSIZE == 64
#define __PRIo64 "lo"
#define __PRIu64 "lu"
#define __PRIx64 "lx"
#define __PRIX64 "lX"
#define __SCNo64 "lo"
#define __SCNu64 "lu"
#define __SCNx64 "lx"
#else
#define __PRIo64 "llo"
#define __PRIu64 "llu"
#define __PRIx64 "llx"
#define __PRIX64 "llX"
#define __SCNo64 "llo"
#define __SCNu64 "llu"
#define __SCNx64 "llx"
#endif

/* Define small signed types. */
typedef signed char __int_least8_t;
typedef short int __int_least16_t;
typedef int __int_least32_t;
#if __WORDSIZE == 64
typedef long int __int_least64_t;
#else
typedef long long int __int_least64_t;
#endif

#define __INT_LEAST8_MIN (-128)
#define __INT_LEAST16_MIN (-32767-1)
#define __INT_LEAST32_MIN (-2147483647-1)
#define __INT_LEAST64_MIN (-__INT64_C(9223372036854775807)-1)

#define __INT_LEAST8_MAX (127)
#define __INT_LEAST16_MAX (32767)
#define __INT_LEAST32_MAX (2147483647)
#define __INT_LEAST64_MAX (__INT64_C(9223372036854775807))

#define __PRIdLEAST8 "d"
#define __PRIiLEAST8 "i"
#define __SCNdLEAST8 "hhd"
#define __SCNiLEAST8 "hhi"
#define __PRIdLEAST16 "d"
#define __PRIiLEAST16 "i"
#define __SCNdLEAST16 "hd"
#define __SCNiLEAST16 "hi"
#define __PRIdLEAST32 "d"
#define __PRIiLEAST32 "i"
#define __SCNdLEAST32 "d"
#define __SCNiLEAST32 "i"
#if __WORDSIZE == 64
#define __PRIdLEAST64 "ld"
#define __PRIiLEAST64 "li"
#define __SCNdLEAST64 "ld"
#define __SCNiLEAST64 "li"
#else
#define __PRIdLEAST64 "lld"
#define __PRIiLEAST64 "lli"
#define __SCNdLEAST64 "lld"
#define __SCNiLEAST64 "lli"
#endif

/* Define small unsigned types. */
typedef unsigned char __uint_least8_t;
typedef unsigned short int __uint_least16_t;
typedef unsigned int __uint_least32_t;
#if __WORDSIZE == 64
typedef unsigned long int __uint_least64_t;
#else
typedef unsigned long long int __uint_least64_t;
#endif

#define __UINT_LEAST8_MAX (255)
#define __UINT_LEAST16_MAX (65535)
#define __UINT_LEAST32_MAX (4294967295U)
#define __UINT_LEAST64_MAX (__UINT64_C(18446744073709551615))

#define __PRIoLEAST8 "o"
#define __PRIuLEAST8 "u"
#define __PRIxLEAST8 "x"
#define __PRIXLEAST8 "X"
#define __SCNoLEAST8 "hho"
#define __SCNuLEAST8 "hhu"
#define __SCNxLEAST8 "hhx"
#define __PRIoLEAST16 "o"
#define __PRIuLEAST16 "u"
#define __PRIxLEAST16 "x"
#define __PRIXLEAST16 "X"
#define __SCNoLEAST16 "ho"
#define __SCNuLEAST16 "hu"
#define __SCNxLEAST16 "hx"
#define __PRIoLEAST32 "o"
#define __PRIuLEAST32 "u"
#define __PRIxLEAST32 "x"
#define __PRIXLEAST32 "X"
#define __SCNoLEAST32 "o"
#define __SCNuLEAST32 "u"
#define __SCNxLEAST32 "x"
#if __WORDSIZE == 64
#define __PRIoLEAST64 "lo"
#define __PRIuLEAST64 "lu"
#define __PRIxLEAST64 "lx"
#define __PRIXLEAST64 "lX"
#define __SCNoLEAST64 "lo"
#define __SCNuLEAST64 "lu"
#define __SCNxLEAST64 "lx"
#else
#define __PRIoLEAST64 "llo"
#define __PRIuLEAST64 "llu"
#define __PRIxLEAST64 "llx"
#define __PRIXLEAST64 "llX"
#define __SCNoLEAST64 "llo"
#define __SCNuLEAST64 "llu"
#define __SCNxLEAST64 "llx"
#endif

/* Define fast signed types. */
typedef signed char __int_fast8_t;
#if __WORDSIZE == 64
typedef long int __int_fast16_t;
typedef long int __int_fast32_t;
typedef long int __int_fast64_t;
#else
typedef int __int_fast16_t;
typedef int __int_fast32_t;
typedef long long int __int_fast64_t;
#endif

#define __INT_FAST8_MIN (-128)
#if __WORDSIZE == 64
#define __INT_FAST16_MIN (-9223372036854775807L-1)
#define __INT_FAST32_MIN (-9223372036854775807L-1)
#else
#define __INT_FAST16_MIN (-2147483647-1)
#define __INT_FAST32_MIN (-2147483647-1)
#endif
#define __INT_FAST64_MIN (-__INT64_C(9223372036854775807)-1)

#define __INT_FAST8_MAX (127)
#if __WORDSIZE == 64
#define __INT_FAST16_MAX (9223372036854775807L)
#define __INT_FAST32_MAX (9223372036854775807L)
#else
#define __INT_FAST16_MAX (2147483647)
#define __INT_FAST32_MAX (2147483647)
#endif
#define __INT_FAST64_MAX (__INT64_C(9223372036854775807))

#define __PRIdFAST8 "d"
#define __PRIiFAST8 "i"
#define __SCNdFAST8 "hhd"
#define __SCNiFAST8 "hhi"
#if __WORDSIZE == 64
#define __PRIdFAST16 "ld"
#define __PRIiFAST16 "li"
#define __SCNdFAST16 "ld"
#define __SCNiFAST16 "li"
#define __PRIdFAST32 "ld"
#define __PRIiFAST32 "li"
#define __SCNdFAST32 "ld"
#define __SCNiFAST32 "li"
#define __PRIdFAST64 "ld"
#define __PRIiFAST64 "li"
#define __SCNdFAST64 "ld"
#define __SCNiFAST64 "li"
#else
#define __PRIdFAST16 "d"
#define __PRIiFAST16 "i"
#define __SCNdFAST16 "d"
#define __SCNiFAST16 "i"
#define __PRIdFAST32 "i"
#define __PRIiFAST32 "d"
#define __SCNdFAST32 "d"
#define __SCNiFAST32 "i"
#define __PRIdFAST64 "lld"
#define __PRIiFAST64 "lli"
#define __SCNdFAST64 "lld"
#define __SCNiFAST64 "lli"
#endif

/* Define fast unsigned types. */
typedef unsigned char __uint_fast8_t;
#if __WORDSIZE == 64
typedef unsigned long int __uint_fast16_t;
typedef unsigned long int __uint_fast32_t;
typedef unsigned long int __uint_fast64_t;
#else
typedef unsigned int __uint_fast16_t;
typedef unsigned int __uint_fast32_t;
typedef unsigned long long int __uint_fast64_t;
#endif

#define __UINT_FAST8_MAX (255)
#if __WORDSIZE == 64
#define __UINT_FAST16_MAX (18446744073709551615UL)
#define __UINT_FAST32_MAX (18446744073709551615UL)
#else
#define __UINT_FAST16_MAX (4294967295U)
#define __UINT_FAST32_MAX (4294967295U)
#endif
#define __UINT_FAST64_MAX (__UINT64_C(18446744073709551615))

#define __PRIoFAST8 "o"
#define __PRIuFAST8 "u"
#define __PRIxFAST8 "x"
#define __PRIXFAST8 "X"
#define __SCNoFAST8 "hho"
#define __SCNuFAST8 "hhu"
#define __SCNxFAST8 "hhx"
#if __WORDSIZE == 64
#define __PRIoFAST16 "lo"
#define __PRIuFAST16 "lu"
#define __PRIxFAST16 "lx"
#define __PRIXFAST16 "lX"
#define __SCNoFAST16 "lo"
#define __SCNuFAST16 "lu"
#define __SCNxFAST16 "lx"
#define __PRIoFAST32 "lo"
#define __PRIuFAST32 "lu"
#define __PRIxFAST32 "lx"
#define __PRIXFAST32 "lX"
#define __SCNoFAST32 "lo"
#define __SCNuFAST32 "lu"
#define __SCNxFAST32 "lx"
#define __PRIoFAST64 "lo"
#define __PRIuFAST64 "lu"
#define __PRIxFAST64 "lx"
#define __PRIXFAST64 "lX"
#define __SCNoFAST64 "lo"
#define __SCNuFAST64 "lu"
#define __SCNxFAST64 "lx"
#else
#define __PRIoFAST16 "o"
#define __PRIuFAST16 "u"
#define __PRIxFAST16 "x"
#define __PRIXFAST16 "X"
#define __SCNoFAST16 "o"
#define __SCNuFAST16 "u"
#define __SCNxFAST16 "x"
#define __PRIoFAST32 "o"
#define __PRIuFAST32 "u"
#define __PRIxFAST32 "x"
#define __PRIXFAST32 "X"
#define __SCNoFAST32 "o"
#define __SCNuFAST32 "u"
#define __SCNxFAST32 "x"
#define __PRIoFAST64 "llo"
#define __PRIuFAST64 "llu"
#define __PRIxFAST64 "llx"
#define __PRIXFAST64 "llX"
#define __SCNoFAST64 "llo"
#define __SCNuFAST64 "llu"
#define __SCNxFAST64 "llx"
#endif

/* Define pointer-safe types. */
#if __WORDSIZE == 64
typedef long int __intptr_t;
typedef unsigned long int __uintptr_t;
#else
typedef int __intptr_t;
typedef unsigned int __uintptr_t;
#endif

#if __WORDSIZE == 64
#define __INTPTR_MIN (-9223372036854775807L-1)
#define __INTPTR_MAX (9223372036854775807L)
#define __UINTPTR_MAX (18446744073709551615UL)
#else
#define __INTPTR_MIN (-2147483647-1)
#define __INTPTR_MAX (2147483647)
#define __UINTPTR_MAX (4294967295U)
#endif

#if __WORDSIZE == 64
#define __PRIdPTR "ld"
#define __PRIiPTR "li"
#define __PRIoPTR "lo"
#define __PRIuPTR "lu"
#define __PRIxPTR "lx"
#define __PRIXPTR "lX"
#define __SCNdPTR "ld"
#define __SCNiPTR "li"
#define __SCNoPTR "lo"
#define __SCNuPTR "lu"
#define __SCNxPTR "lx"
#else
#define __PRIdPTR "d"
#define __PRIiPTR "i"
#define __PRIoPTR "o"
#define __PRIuPTR "u"
#define __PRIxPTR "x"
#define __PRIXPTR "X"
#define __SCNdPTR "d"
#define __SCNiPTR "i"
#define __SCNoPTR "o"
#define __SCNuPTR "u"
#define __SCNxPTR "x"
#endif

/* Define largest integer types. */
#if __WORDSIZE == 64
typedef long int __intmax_t;
typedef unsigned long int __uintmax_t;
#else
typedef long long int __intmax_t;
typedef long long unsigned int __uintmax_t;
#endif

#define __INTMAX_MIN (-__INT64_C(9223372036854775807)-1)
#define __INTMAX_MAX (__INT64_C(9223372036854775807))
#define __UINTMAX_MAX (__UINT64_C(18446744073709551615))

#if __WORDSIZE == 64
#define __PRIdMAX "ld"
#define __PRIiMAX "li"
#define __PRIoMAX "lo"
#define __PRIuMAX "lu"
#define __PRIxMAX "lx"
#define __PRIXMAX "lX"
#define __SCNdMAX "ld"
#define __SCNiMAX "li"
#define __SCNoMAX "lo"
#define __SCNuMAX "lu"
#define __SCNxMAX "lx"
#else
#define __PRIdMAX "lld"
#define __PRIiMAX "lli"
#define __PRIoMAX "llo"
#define __PRIuMAX "llu"
#define __PRIxMAX "llx"
#define __PRIXMAX "llX"
#define __SCNdMAX "lld"
#define __SCNiMAX "lli"
#define __SCNoMAX "llo"
#define __SCNuMAX "llu"
#define __SCNxMAX "llx"
#endif

/* TODO: Should these come from a <__stddef.h>? */
#if __WORDSIZE == 64
#define __PTRDIFF_MIN (-9223372036854775807L-1)
#define __PTRDIFF_MAX (9223372036854775807L)
#else
#define __PTRDIFF_MIN (-2147483647-1)
#define __PTRDIFF_MAX (2147483647)
#endif

/* Note: The wchar_t related constants comes from another header. */

/* TODO: Should these come from a <__signal.h>? */
#define __SIG_ATOMIC_MIN (-2147483647-1)
#define __SIG_ATOMIC_MAX (2147483647)

/* TODO: Should these come from a <__stddef.h>? */
#define __SIZE_MAX __SIZE_MAX__

__END_DECLS

#endif
