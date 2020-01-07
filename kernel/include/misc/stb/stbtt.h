// stbtt.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stddef.h>
#include "stdint.h"

#include "nx.h"

typedef uint8_t     stbtt_uint8;
typedef uint16_t    stbtt_uint16;
typedef uint32_t    stbtt_uint32;

typedef int8_t      stbtt_int8;
typedef int16_t     stbtt_int16;
typedef int32_t     stbtt_int32;


#define STBTT_malloc(x,u)   ((void) (u), (void*) nx::heap::allocate((x), 1))
#define STBTT_free(x,u)     ((void) (u), nx::heap::deallocate((addr_t) (x)))

#define STBTT_assert(x)     assert(x)
#define STBTT_strlen(x)     strlen(x)

#define STBTT_memcpy        memcpy
#define STBTT_memset        memset

// we have libm, but because we are building the kernel without using SSE/MMX on x64, we must
// use -msoft-float. this means we cannot use <long double>. we don't actually use it, but
// pulling in math.h directly will cause compile errors in its long-double declarations. so,
// manually declare these.

// extern "C" float floor(float x);
// extern "C" float ceil(float x);
// extern "C" float sqrt(float x);
// extern "C" float pow(float x, float y);
// extern "C" float fmod(float x, float y);
// extern "C" float cos(float x);
// extern "C" float acos(float x);
// extern "C" float fabs(float x);

// #define STBTT_ifloor        floor
// #define STBTT_iceil         ceil
// #define STBTT_sqrt          sqrt
// #define STBTT_pow           pow
// #define STBTT_fmod          fmod
// #define STBTT_cos           cos
// #define STBTT_acos          acos
// #define STBTT_fabs          fabs

#include "stb_truetype.h"
