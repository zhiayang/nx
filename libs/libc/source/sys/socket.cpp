// socket.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "../../include/sys/socket.h"
#include "../../include/netinet/in.h"
#include "../../include/sys/un.h"
#include "../../include/errno.h"
#include "../../include/assert.h"
#include "../../include/orionx/syscall.h"

extern "C" int socket(int domain, int type, int protocol)
{
	return Library::SystemCall::OpenSocket(domain, type, protocol);
}




static void sockAddrToIPv4(const struct sockaddr* addr, uint32_t* ipv4, uint16_t* port)
{
	assert(addr);
	assert(addr->sa_family == AF_INET);

	sockaddr_in* sockin = (sockaddr_in*) addr;

	*ipv4 = sockin->sin_addr.s_addr;
	*port = sockin->sin_port;
}

// todo: fix dupe code?
extern "C" int bind(int socket, const struct sockaddr* addr, socklen_t addr_len)
{
	if(!addr || addr_len == 0)
	{
		errno = EFAULT;
		return -1;
	}

	// do some work in userspace for a change
	if(addr->sa_family == AF_INET)
	{
		uint32_t ipv4 = 0;
		uint16_t port = 0;

		sockAddrToIPv4(addr, &ipv4, &port);
		return Library::SystemCall::BindNetSocket(socket, ipv4, port);
	}
	else if(addr->sa_family == AF_UNIX)
	{
		sockaddr_un* un = (sockaddr_un*) addr;
		return Library::SystemCall::BindIPCSocket(socket, un->sun_path);
	}
	else
	{
		errno = EAFNOSUPPORT;
		return -1;
	}
}

extern "C" int connect(int socket, const struct sockaddr* addr, socklen_t addr_len)
{
	if(!addr || addr_len == 0)
	{
		errno = EFAULT;
		return -1;
	}

	// do some work in userspace for a change
	if(addr->sa_family == AF_INET)
	{
		uint32_t ipv4 = 0;
		uint16_t port = 0;

		sockAddrToIPv4(addr, &ipv4, &port);
		return Library::SystemCall::ConnectNetSocket(socket, ipv4, port);
	}
	else if(addr->sa_family == AF_UNIX)
	{
		sockaddr_un* un = (sockaddr_un*) addr;
		return Library::SystemCall::ConnectIPCSocket(socket, un->sun_path);
	}
	else
	{
		errno = EAFNOSUPPORT;
		return -1;
	}
}









// extern "C" int listen(int socket, int backlog)
// {

// }

// extern "C" int accept(int socket, struct sockaddr* addr, socklen_t* addr_len)
// {

// }

// extern "C" int socketpair(int domain, int type, int protocol, int socket_vec[2])
// {

// }
























