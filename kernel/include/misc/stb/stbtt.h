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

// we don't need to override the math functions -- we have libm.

#define STBTT_malloc(x,u)  ((void) (u), (void*) nx::heap::allocate((x), 1))
#define STBTT_free(x,u)    ((void) (u), nx::heap::deallocate((addr_t) (x)))

#define STBTT_assert(x)    assert(x)
#define STBTT_strlen(x)    strlen(x)

#define STBTT_memcpy       memcpy
#define STBTT_memset       memset

#include "stb_truetype.h"
