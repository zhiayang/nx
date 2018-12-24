// unistd.h
// Copyright (c) 2014 - 2016, zhiayang
// Licensed under the Apache License Version 2.0.

#include "stdint.h"
#include <stddef.h>
#include "sys/types.h"
#include "stdio.h"

#pragma once
#ifndef __unistd_h
#define __unistd_h

#define _POSIX_VERSION 	200809L /* C bindings */
#define _POSIX2_VERSION	200809L /* Shell utilities. */

#undef _POSIX_THREADS

#define STDIN_FILENO	0
#define STDOUT_FILENO	1
#define STDERR_FILENO	2

#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2


#ifdef __cplusplus
extern "C" {
#endif

int close(int fd);

ssize_t write(int fildes, const void* buf, size_t nbyte);
ssize_t read(int fd, void* buf, size_t count);

pid_t getppid();
pid_t getpid();

off_t lseek(int fildes, off_t offset, int whence);
off_t tell(int filedes);

int usleep(useconds_t usec);

pid_t fork();
int execv(const char*, char* const[]);
int execve(const char*, char* const[], char* const[]);
int execvp(const char*, char* const[]);

#ifdef __cplusplus
}
#endif

#endif










