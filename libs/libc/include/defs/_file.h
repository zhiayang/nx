// _file.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#ifndef __file_h
#define __file_h
#pragma once

#include "../stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

#define __LASTOP_READ	1
#define __LASTOP_WRITE	2


struct _FILE
{
	uint64_t __fd;
	uint8_t __PermFlags;

	// buffer stuff
	uint64_t bufferoffset;
	uint64_t bufferfill;
	uint64_t buffersize;
	uint64_t bufferread;
	uint8_t* buffer;


	uint8_t bufmode;
	uint8_t lastop;
	uint8_t indicatorflags;
};

typedef struct _FILE FILE;

#ifdef __cplusplus
}
#endif

#endif
