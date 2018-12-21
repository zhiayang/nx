// mqueue.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "sys/ipc.h"
#include "sys/types.h"
#include "fcntl.h"

__BEGIN_DECLS


#define __PREFIX_LENGTH	9	// "/dev/_mq/"

typedef long mqd_t;
struct mq_attr
{
	long mq_flags;
	long mq_maxmsg;
	long mq_msgsize;
	long mq_curmsgs;
};

int mq_close(mqd_t);
int mq_getattr(mqd_t, struct mq_attr*);
int mq_notify(mqd_t, const struct sigevent*);
mqd_t mq_open(const char*, int, ...);
ssize_t mq_receive(mqd_t, char*, size_t, unsigned int*);
int mq_send(mqd_t, const char *, size_t, unsigned int);
int mq_setattr(mqd_t, const struct mq_attr*, struct mq_attr*);
int mq_unlink(const char*);

__END_DECLS
