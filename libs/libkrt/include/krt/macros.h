// macros.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#define __min(a, b)     ((a) > (b) ? (b) : (a))
#define __max(a, b)     ((a) > (b) ? (a) : (b))
#define __abs(x)        ((x) < (0) ? (-(x)) : (x))

#define __alignup(x, y) ((((x) + ((y) - 1)) / (y)) * (y))

#define KB(x)           (1024 * (x))
#define MB(x)           (1024 * KB(x))
#define GB(x)           (1024 * MB(x))
