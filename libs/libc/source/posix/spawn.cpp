// spawn.cpp
// Copyright (c) 2014 - 2016, zhiayang
// Licensed under the Apache License Version 2.0.

#if 0

#include "../include/spawn.h"
#include "../../include/orionx/syscall.h"


int posix_spawn(pid_t* restrict pid, const char* restrict path, const posix_spawn_file_actions_t* file_actions,	const posix_spawnattr_t* restrict attrp, char* const argv[], char* const envp[])
{
	pid_t proc_pid = Library::SystemCall::SpawnProcess(path, path);

	if(pid)
		*pid = proc_pid;

	return proc_pid ? 0 : -1;
}

int posix_spawnp(pid_t* restrict pid, const char* restrict file, const posix_spawn_file_actions_t* file_actions, const posix_spawnattr_t* restrict attrp, char* const argv[], char* const envp[])
{
	return posix_spawn(pid, file, file_actions, attrp, argv, envp);
}

#endif
