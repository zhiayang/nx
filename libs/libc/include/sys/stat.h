// stat.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "types.h"

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#ifndef __stat_struct
#define __stat_struct
struct stat
{
	dev_t		st_dev;		// ID of device containing file
	ino_t		st_ino;		// inode number
	mode_t	st_mode;	// protection
	nlink_t		st_nlink;	// number of hard links
	uid_t		st_uid;		// user ID of owner
	gid_t		st_gid;		// group ID of owner
	dev_t		st_rdev;	// device ID (if special file)
	off_t		st_size;		// total size, in bytes
	blksize_t	st_blksize;	// blocksize for file system I/O
	blkcnt_t	st_blocks;	// number of 512B blocks allocated
	time_t		st_atime;	// time of last access
	time_t		st_mtime;	// time of last modification
	time_t		st_ctime;	// time of last status change
};
#endif


int stat(const char* path, struct stat* buf);
int fstat(int fd, struct stat* buf);
int lstat(const char* path, struct stat* buf);


#ifdef __cplusplus
}
#endif
