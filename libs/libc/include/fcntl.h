// fcntl.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "stdint.h"
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// TODO: actually use these flags.
#define O_READ		(1 << 0)
#define O_WRITE		(1 << 1)
#define O_CREATE		(1 << 2)
#define O_TRUNC		(1 << 3)
#define O_APPEND		(1 << 4)

#define O_RDONLY		O_READ
#define O_WRONLY		O_WRITE
#define O_RDWR		(O_RDONLY | O_WRONLY)
#define O_CREAT		O_CREATE

int open(const char* path, int flags, ...);

#ifdef __cplusplus
}
#endif
