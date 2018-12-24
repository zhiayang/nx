// misc.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdarg.h>

#include "krt.h"
#include "efx.h"


static size_t cb_efiprint(void* ctx, const char* s, size_t len)
{
}

extern "C" int efiprintf(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	int ret = vcbprintf(nullptr, cb_efiprint, fmt, args);
	va_end(args);

	return ret;
}











constexpr size_t BUFFER_LEN = 256;
static char16_t buffer[BUFFER_LEN];

extern "C" char16_t* efistr(char* inp, size_t len)
{
	size_t i = 0;
	for(; i < __min(len, BUFFER_LEN); i++)
		buffer[i] = inp[i];

	buffer[i] = 0;
	return &buffer[0];
}


