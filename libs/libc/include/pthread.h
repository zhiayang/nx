// pthread.h
// Copyright (c) 2014 - 2016, zhiayang
// Licensed under the Apache License Version 2.0.

#ifndef __pthread_h
#define __pthread_h

#include "sys/cdefs.h"

__BEGIN_DECLS

#include "stddef.h"
#include "stdint.h"
#include "sys/types.h"
#include "defs/_pthreadstructs.h"


#define PTHREAD_MUTEX_DEFAULT		PTHREAD_MUTEX_ERRORCHECK
#define PTHREAD_MUTEX_ERRORCHECK	0
#define PTHREAD_MUTEX_RECURSIVE		1
#define PTHREAD_MUTEX_NORMAL		2

int pthread_create(pthread_t* restrict thread, const pthread_attr_t* restrict attr, void *(*start_routine)(void*), void *restrict arg);
int pthread_join(pthread_t thread, void** retval);
pthread_t pthread_self();

int pthread_mutex_init(pthread_mutex_t* mutex, const pthread_mutexattr_t* attr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
int pthread_mutex_lock(pthread_mutex_t* mutex);
int pthread_mutex_trylock(pthread_mutex_t* mutex);
int pthread_mutex_unlock(pthread_mutex_t* mutex);







__END_DECLS
#endif
