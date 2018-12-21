// dlfcn.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#ifndef _LIBC_CTYPE_H_
#define _LIBC_CTYPE_H_

#ifdef __cplusplus
extern "C" {
#endif

void* dlopen(const char* filename, int flag);
char* dlerror(void);
void* dlsym(void* handle, const char* symbol);
int dlclose(void* handle);

#ifdef __cplusplus
}
#endif

#endif