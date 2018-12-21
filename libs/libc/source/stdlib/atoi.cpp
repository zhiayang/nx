// atoi.cpp
// Copyright (c) 2011 - 2014, Jonas 'Sortie' Termansen
// Licensed under the GNU LGPL.

#include "../../include/stdlib.h"

extern "C" long long atoll(const char* str)
{
	return strtoll(str, (char**) NULL, 10);
}

extern "C" long atol(const char* str)
{
	return strtol(str, (char**) NULL, 10);
}

extern "C" int atoi(const char* str)
{
	return (int) strtol(str, (char**) NULL, 10);
}
extern "C" double atof(const char* str)
{
	return strtod(str, NULL);
}


