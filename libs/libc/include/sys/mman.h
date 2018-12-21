// mman.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.


#include <stddef.h>
#include "../stdint.h"
#include "./types.h"


#define PROT_EXEC		(1<<0)
#define PROT_WRITE		(1<<1)
#define PROT_READ		(1<<2)
#define PROT_USER		(PROT_EXEC | PROT_WRITE | PROT_READ)

#define MAP_SHARED		(1<<0)
#define MAP_PRIVATE		(1<<1)

#define MAP_ANONYMOUS	(1<<2)
#define MAP_FIXED		(1<<3)
#define MAP_FAILED		((void*) -1)

// deprecated:
#define MAP_ANON		MAP_ANONYMOUS


#ifdef __cplusplus
extern "C" {
#endif

void* mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int munmap(void *addr, size_t length);

#ifdef __cplusplus
}
#endif
