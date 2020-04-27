// dlfcn.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#ifndef _LIBC_DLFCN_H_
#define _LIBC_DLFCN_H_

#ifdef __cplusplus
extern "C" {
#endif

#define RTLD_LAZY   (1<<0)
#define RTLD_NOW    (1<<1)
#define RTLD_GLOBAL (1<<8)
#define RTLD_LOCAL  0


void* dlopen(const char* filename, int flag);
char* dlerror(void);
void* dlsym(void* handle, const char* symbol);
int dlclose(void* handle);

#ifdef __cplusplus
}
#endif

#endif

