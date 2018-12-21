// errno.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#ifndef _STDC_ERRNO_H_
#define _STDC_ERRNO_H_

#include "sys/types.h"
#include "sys/cdefs.h"
#include "defs/_errnos.h"

__BEGIN_DECLS

extern /*__thread*/ int _errno;
#define errno			_errno

__END_DECLS
#define EOPNOTSUPP ENOTSUP
#endif

