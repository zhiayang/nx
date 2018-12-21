// strtoull.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.


#pragma GCC diagnostic push

#pragma GCC diagnostic ignored "-Winteger-overflow"
#pragma GCC diagnostic ignored "-Wshift-count-overflow"

#define STRTOL strtoull
#define STRTOL_CHAR char
#define STRTOL_L(x) x
#define STRTOL_ISSPACE isspace
#define STRTOL_INT unsigned long long
#define STRTOL_UNSIGNED_INT unsigned long long
#define STRTOL_INT_MIN 0
#define STRTOL_INT_MAX ULLONG_MAX
#define STRTOL_INT_IS_UNSIGNED true

#include "strtol.cpp"

#pragma GCC diagnostic pop
