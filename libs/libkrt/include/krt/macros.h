// macros.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#define __min(a ,b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })
#define __max(a ,b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#define __abs(a)        ({ __typeof__ (a) _a = (a); _a < 0 ? -_a : _a; })


#define __alignup(x, y) ((((x) + ((y) - 1)) / (y)) * (y))

#define KB(x)           (1024 * (x))
#define MB(x)           (1024 * KB(x))
#define GB(x)           (1024 * MB(x))
