// uio.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "types.h"


#pragma once
#ifndef __uio_h
#define __uio_h

__BEGIN_DECLS

struct iovec
{
	void* iov_base;
	size_t iov_len;
};

ssize_t readv(int fd, const struct iovec* iov, int iovcnt);
ssize_t writev(int fd, const struct iovec* iov, int iovcnt);

__END_DECLS

#endif
