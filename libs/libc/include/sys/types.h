// types.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <stddef.h>
#include "../stdint.h"

#include "../defs/_pthreadstructs.h"

#pragma once

#ifndef __blkcnt_t
#define __blkcnt_t
typedef uint64_t blkcnt_t;
#endif

#ifndef __blksize_t
#define __blksize_t
typedef uint64_t blksize_t;
#endif

#ifndef __clock_t
#define __clock_t
typedef uint64_t clock_t;
#endif

#ifndef __clockid_t
#define __clockid_t
typedef uint64_t clockid_t;
#endif

#ifndef __dev_t
#define __dev_t
typedef uint64_t dev_t;
#endif

#ifndef __fsblkcnt_t
#define __fsblkcnt_t
typedef uint64_t fsblkcnt_t;
#endif

#ifndef __fsfilcnt_t
#define __fsfilcnt_t
typedef uint64_t fsfilcnt_t;
#endif

#ifndef __gid_t
#define __gid_t
typedef uint64_t gid_t;
#endif

#ifndef __id_t
#define __id_t
typedef uint64_t id_t;
#endif

#ifndef __ino_t
#define __ino_t
typedef uint64_t ino_t;
#endif

#ifndef __clock_t
#define __clock_t
typedef uint64_t clock_t;
#endif

#ifndef __key_t
#define __key_t
typedef uint64_t key_t;
#endif

#ifndef __mode_t
#define __mode_t
typedef uint64_t mode_t;
#endif

#ifndef __nlink_t
#define __nlink_t
typedef uint64_t nlink_t;
#endif

#ifndef __off_t
#define __off_t
typedef size_t off_t;
#endif

#ifndef __pid_t
#define __pid_t
typedef int64_t pid_t;
#endif

#ifndef __ssize_t
#define __ssize_t
typedef int64_t ssize_t;
#endif

#ifndef __suseconds_t
#define __suseconds_t
typedef int64_t suseconds_t;
#endif

#ifndef __time_t
#define __time_t
typedef int64_t time_t;
#endif

#ifndef __timer_t
#define __timer_t
typedef int64_t timer_t;
#endif

#ifndef __uid_t
#define __uid_t
typedef uint64_t uid_t;
#endif

#ifndef __useconds_t
#define __useconds_t
typedef uint64_t useconds_t;
#endif



#ifndef __err_t
#define __err_t
typedef int err_t;
#endif


// pthreads
#ifndef __pthread_t
#define __pthread_t
typedef pid_t pthread_t;
#endif

#ifndef __pthread_attr_t
#define __pthread_attr_t
typedef struct Thread_attr pthread_attr_t;
#endif

#ifndef __pthread_mutex_t
#define __pthread_mutex_t
typedef uint64_t pthread_mutex_t;
#endif

#ifndef __pthread_mutexattr_t
#define __pthread_mutexattr_t
typedef struct Mutex_attr pthread_mutexattr_t;
#endif









