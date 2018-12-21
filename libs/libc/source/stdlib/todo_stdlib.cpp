// todo_stdlib
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "../../include/stdlib.h"
#include "../../include/string.h"
#include "../../include/stdio.h"
#include "../../include/time.h"

tm __tm;

extern "C" char* getenv(const char* name)
{
	(void) name;

	return NULL;
}

extern "C" int system(const char* cmd)
{
	if(!cmd)
		return 0;

	return -1;
}

extern "C" int strcoll(const char* str1, const char* str2)
{
	return strcmp(str1, str2);
}

extern "C" char* strerror(int errnum)
{
	(void) errnum;
	return (char*) "unknown error";
}

extern "C" size_t strxfrm(char* dest, const char* source, size_t num)
{
	strncpy(dest, source, num);
	return num;
}

extern "C" clock_t clock()
{
	return 0;
}

extern "C" double difftime(time_t a, time_t b)
{
	return a - b;
}

extern "C" time_t time(time_t* timer)
{
	if(timer)
		return *timer;

	return -1;
}

extern "C" char* asctime(const tm* timeptr)
{
	static const char wday_name[][4] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
	static const char mon_name[][4] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	static char result[26];
	sprintf(result, "%.3s %.3s%3d %.2d:%.2d:%.2d %d\n", wday_name[timeptr->tm_wday], mon_name[timeptr->tm_mon], timeptr->tm_mday,
		timeptr->tm_hour, timeptr->tm_min, timeptr->tm_sec, 1900 + timeptr->tm_year);

	return result;
}


extern "C" tm* localtime(const time_t* timer)
{
	(void) timer;
	return (tm*) malloc(sizeof(tm));
}



extern "C" char* ctime(const time_t* timer)
{
	return asctime(localtime(timer));
}

extern "C" size_t strftime(char* ptr, size_t maxsize, const char* fmt, const struct tm* timeptr)
{
	(void) ptr;
	(void) maxsize;
	(void) fmt;
	(void) timeptr;

	return 0;
}




extern "C" size_t mbstowcs(wchar_t* dest, const char* src, size_t max)
{
	(void) dest;
	(void) src;
	(void) max;

	return -1;
}

extern "C" int mbtowc(wchar_t* pwc, const char* pmb, size_t max)
{
	(void) pwc;
	(void) pmb;
	(void) max;

	return -1;
}
