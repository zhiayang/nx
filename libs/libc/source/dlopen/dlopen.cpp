// stubs.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "dlfcn.h"
extern "C" {

static const char* dlerrormsg = 0;

void* dlopen(const char* filename, int mode)
{
	dlerrormsg = "dynamic linking is not supported atm";
	return 0;
}

void* dlsym(void* handle, const char* name)
{
	dlerrormsg = "dynamic linking is not supported atm";
	return 0;
}

int dlclose(void* handle)
{
	return 0;
}

char* dlerror()
{
	const char* result = dlerrormsg;
	dlerrormsg = 0;

	return (char*) result;
}



}