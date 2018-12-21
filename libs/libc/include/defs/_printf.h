// _printf.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "../stdint.h"
#include <stddef.h>
#include <stdarg.h>

#ifndef __printf_h
#define __printf_h

extern "C" int vcbprintf(void* ctx, size_t (*callback)(void*, const char*, size_t), const char* format, va_list parameters);

#endif
