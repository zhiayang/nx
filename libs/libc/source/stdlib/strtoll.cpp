// strtoll.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.


#define STRTOL strtoll
#define STRTOL_CHAR char
#define STRTOL_L(x) x
#define STRTOL_ISSPACE isspace
#define STRTOL_INT long long
#define STRTOL_UNSIGNED_INT unsigned long long
#define STRTOL_INT_MIN LLONG_MIN
#define STRTOL_INT_MAX LLONG_MAX
#define STRTOL_INT_IS_UNSIGNED false

#include "strtol.cpp"
