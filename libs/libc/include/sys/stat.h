// stat.h
// Copyright (c) 2014 - 2016, zhiayang
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

#define S_IXOTH 01
#define S_IWOTH 02
#define S_IROTH 04
#define S_IRWXO 07
#define S_IXGRP 010
#define S_IWGRP 020
#define S_IRGRP 040
#define S_IRWXG 070
#define S_IXUSR 0100
#define S_IWUSR 0200
#define S_IRUSR 0400
#define S_IRWXU 0700


int stat(const char* path, struct stat* buf);
int fstat(int fd, struct stat* buf);
int lstat(const char* path, struct stat* buf);

int chmod(const char* pathname, mode_t mode);
int fchmod(int fd, mode_t mode);

int mkdir(const char* pathname, mode_t mode);


#ifdef __cplusplus
}
#endif






