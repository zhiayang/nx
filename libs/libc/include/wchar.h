// wchar.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

/* Conversion state information. */
typedef struct
{
	unsigned short count;
	unsigned short length;
	int wch;

} mbstate_t;



#ifdef __cplusplus
}
#endif
