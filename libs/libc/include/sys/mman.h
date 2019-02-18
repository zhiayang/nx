// mman.h
// Copyright (c) 2014 - 2016, zhiayang
// Licensed under the Apache License Version 2.0.


#include <stddef.h>
#include "../stdint.h"
#include "./types.h"
#include "./cdefs.h"

#define PROT_NONE               (0)
#define PROT_EXEC               (1 << 0)
#define PROT_WRITE              (1 << 1)
#define PROT_READ               (1 << 2)

#define MAP_SHARED              (1 << 0)
#define MAP_PRIVATE             (1 << 1)

#define MAP_ANONYMOUS           (1 << 2)
#define MAP_FIXED               (1 << 3)
#define MAP_FIXED_NOREPLACE     (1 << 4)

#define MAP_FAILED              ((void*) -1)

// deprecated:
#define MAP_ANON                MAP_ANONYMOUS
#define MAP_FILE                0
#define MAP_DENYWRITE           0
#define MAP_EXECUTABLE          0

__BEGIN_DECLS

void* mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int munmap(void *addr, size_t length);

__END_DECLS
