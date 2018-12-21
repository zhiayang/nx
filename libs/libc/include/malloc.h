// malloc.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "sys/cdefs.h"
#include "stddef.h"

__BEGIN_DECLS

// malloc and friends
void*	malloc(size_t size);
void	free(void* ptr);
void*	calloc(size_t num, size_t size);
void*	realloc(void* ptr, size_t newsize);

void*	memalign(size_t alignment, size_t size);
void*	pvalloc(size_t size);

__END_DECLS
