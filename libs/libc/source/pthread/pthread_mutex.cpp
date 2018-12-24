// pthread_mutex.cpp
// Copyright (c) 2014 - 2016, zhiayang
// Licensed under the Apache License Version 2.0.

#include <pthread.h>
#include "../../include/orionx/syscall.h"
#include <errno.h>

extern "C" int pthread_mutex_init(pthread_mutex_t* mutex, const pthread_mutexattr_t* attr)
{
	errno = 0;
	Library::SystemCall::CreateMutex(mutex, attr);

	return errno != 0 ? -1 : 0;
}

extern "C" int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
	errno = 0;
	Library::SystemCall::DestroyMutex(mutex);

	return errno != 0 ? -1 : 0;
}

extern "C" int pthread_mutex_lock(pthread_mutex_t* mutex)
{
	return (int) Library::SystemCall::LockMutex(mutex);
}

extern "C" int pthread_mutex_trylock(pthread_mutex_t* mutex)
{
	return (int) Library::SystemCall::TryLockMutex(mutex);
}

extern "C" int pthread_mutex_unlock(pthread_mutex_t* mutex)
{
	return (int) Library::SystemCall::UnlockMutex(mutex);
}





