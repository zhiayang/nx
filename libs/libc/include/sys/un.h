// un.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.


#ifndef __un_h
#define __un_h

#include <sys/socket.h>

__BEGIN_DECLS
struct sockaddr_un
{
	sa_family_t sun_family;
	char sun_path[128 - sizeof(sa_family_t)];
};

__END_DECLS
#endif
