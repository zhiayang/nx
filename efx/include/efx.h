// efx.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdint.h>
#include <stddef.h>

// fuck EFI
#define STR(x) ((char16_t*) L ## x)



extern "C" int efiprintf(const char* fmt, ...);
extern "C" char16_t* efistr(char* inp, size_t len);