// lseek.cpp
// Copyright (c) 2014 - 2016, zhiayang
// Licensed under the Apache License Version 2.0.

#if 0
#include "../../include/unistd.h"
#include "../../include/orionx/syscall.h"

off_t lseek(int fd, off_t offset, int whence)
{
	int ret = Library::SystemCall::Seek(fd, offset, whence);
	return ret == 0 ? tell(fd) : -1;
}

off_t tell(int fd)
{
	return Library::SystemCall::GetSeekPos(fd);
}
#endif
