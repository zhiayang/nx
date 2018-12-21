// time.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#ifndef __time_h
#define __time_h

#include "defs/_file.h"
#include "sys/types.h"

#ifdef __cplusplus
extern "C" {
#endif



#ifndef __has__tm
#define __has__tm
struct tm
{
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};

#ifndef __cplusplus
typedef struct tm tm;
#endif

#endif

#ifndef __clocks_per_second
#define __clocks_per_second
#define CLOCKS_PER_SEC	(1000 * 1000 * 1000)
#endif

#ifndef __timespec
#define __timespec
struct timespec
{
	time_t tv_sec;
	long tv_nsec;
};

#ifndef __cplusplus
typedef struct timespec timespec;
#endif

#endif

#ifndef __clock_realtime
#define __clock_realtime
#define CLOCK_REALTIME	0
#endif


time_t timegm(tm* tm);
tm* gmtime(const time_t* timer);
tm* gmtime_r(const time_t* time_ptr, tm* ret);


clock_t clock();
double difftime(time_t end, time_t beginning);
time_t mktime(tm* tms);
time_t time(time_t* timer);
char* asctime(const tm* timeptr);
tm* localtime(const time_t* timer);
char* ctime(const time_t* timer);
size_t strftime(char* ptr, size_t maxsize, const char* fmt, const struct tm* timeptr);




#ifdef __cplusplus
}
#endif




#endif
