// spawn.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#ifndef __spawn_h
#define __spawn_h

#include "sys/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	uint64_t spawn_flags;

} posix_spawnattr_t;

typedef struct
{
	uint64_t something;

} posix_spawn_file_actions_t;


int posix_spawn(pid_t* restrict pid, const char* restrict path,
	const posix_spawn_file_actions_t* file_actions,
	const posix_spawnattr_t* restrict attrp,
	char* const argv[], char* const envp[]);

int posix_spawnp(pid_t* restrict pid, const char* restrict file,
	const posix_spawn_file_actions_t* file_actions,
	const posix_spawnattr_t* restrict attrp,
	char* const argv[], char* const envp[]);

#ifdef __cplusplus
}
#endif

#endif
