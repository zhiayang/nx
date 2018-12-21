// strtoul.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.


#pragma GCC diagnostic push
// #pragma clang diagnostic push

#pragma GCC diagnostic ignored "-Winteger-overflow"
// #pragma clang diagnostic ignored "-Winteger-overflow"

#pragma GCC diagnostic ignored "-Wshift-count-overflow"
// #pragma clang diagnostic ignored "-Wshift-count-overflow"

#define STRTOL strtoul
#define STRTOL_CHAR char
#define STRTOL_L(x) x
#define STRTOL_ISSPACE isspace
#define STRTOL_INT unsigned long
#define STRTOL_UNSIGNED_INT unsigned long
#define STRTOL_INT_MIN 0
#define STRTOL_INT_MAX ULONG_MAX
#define STRTOL_INT_IS_UNSIGNED true

#include "strtol.cpp"

#pragma GCC diagnostic pop
// #pragma clang diagnostic pop
