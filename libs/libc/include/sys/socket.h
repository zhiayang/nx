// socket.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.


#include "stddef.h"
#include "stdint.h"
#include "types.h"
#include "uio.h"

#pragma once
#ifndef __socket_h
#define __socket_h

__BEGIN_DECLS

#ifndef __socklen_t_defined
#define __socklen_t_defined
typedef uint64_t socklen_t;
#endif

#ifndef __sa_family_t_defined
#define __sa_family_t_defined
typedef unsigned short sa_family_t;
#endif


struct sockaddr
{
	sa_family_t sa_family;
	char sa_data[14];	// 16 - sizeof(sa_family_t)
};

#define	_SS_MAXSIZE		128
#define	_SS_ALIGNSIZE	(sizeof(int64_t))
#define	_SS_PAD1SIZE	(_SS_ALIGNSIZE - sizeof(uint8_t) - sizeof(sa_family_t))
#define	_SS_PAD2SIZE	(_SS_MAXSIZE - sizeof(uint8_t) - sizeof(sa_family_t) - _SS_PAD1SIZE - _SS_ALIGNSIZE)

struct sockaddr_storage
{
	uint8_t ss_len;						// address length
	sa_family_t ss_family;				// address family
	char __ss_pad1[_SS_PAD1SIZE];
	int64_t __ss_align;					// force structure storage alignment
	char __ss_pad2[_SS_PAD2SIZE];
};

struct msghdr
{
	void* msg_name;
	socklen_t msg_namelen;
	struct iovec* msg_iov;
	int msg_iovlen;
	void* msg_control;
	socklen_t msg_controllen;
	int msg_flags;
};

struct cmsghdr
{
	socklen_t cmsg_len;
	int cmsg_level;
	int cmsg_type;
};

#define SCM_RIGHTS 1

/* TODO: CMSG_DATA(cmsg) */
/* TODO: CMSG_NXTHDR(cmsg) */
/* TODO: CMSH_FIRSTHDR(cmsg) */

struct linger
{
	int l_onoff;
	int l_linger;
};

#define SOL_SOCKET 0
/* #define IPPROTO_* constants follow numerically. */

/* Options at the SOL_SOCKET socket level. */
#define SO_ACCEPTCONN	1
#define SO_BROADCAST	2
#define SO_DEBUG		3
#define SO_DONTROUTE	4
#define SO_ERROR		5
#define SO_KEEPALIVE	6
#define SO_LINGER		7
#define SO_OOBINLINE	8
#define SO_RCVBUF		9
#define SO_RCVLOWAT		10
#define SO_RCVTIMEO		11
#define SO_REUSEADDR	12
#define SO_SNDBUF		13
#define SO_SNDLOWAT		14
#define SO_SNDTIMEO		15
#define SO_TYPE			16

#define SOMAXCONN		5

#define MSG_CTRUNC			(1 << 0)
#define MSG_DONTROUTE		(1 << 1)
#define MSG_EOR				(1 << 2)
#define MSG_OOB				(1 << 3)
#define MSG_NOSIGNAL		(1 << 4)
#define MSG_PEEK			(1 << 5)
#define MSG_TRUNC			(1 << 6)
#define MSG_WAITALL			(1 << 7)
#define MSG_DONTWAIT		(1 << 8)
#define MSG_CMSG_CLOEXEC	(1 << 9)

#define AF_UNSPEC			0
#define AF_INET				1
#define AF_INET6			2
#define AF_UNIX				3

#define PF_UNSPEC			AF_UNSPEC
#define PF_INET				AF_INET
#define PF_INET6			AF_INET6
#define PF_UNIX				AF_UNIX

#define AF_LOCAL			AF_UNIX
#define PF_LOCAL			PF_UNIX

/* TODO: Nicely wrap this in an enum, as in glibc's header? */
#define SHUT_RD				(1 << 0)
#define SHUT_WR				(1 << 1)
#define SHUT_RDWR			(SHUT_RD | SHUT_WR)

#define SOCK_RAW			1
#define SOCK_DGRAM			2
#define SOCK_SEQPACKET		3
#define SOCK_STREAM			4

int listen(int socket, int backlog);
int socket(int domain, int type, int protocol);
int accept(int socket, struct sockaddr* addr, socklen_t* addr_len);
int socketpair(int domain, int type, int protocol, int socket_vec[2]);
int bind(int socket, const struct sockaddr* addr, socklen_t addr_len);
int connect(int socket, const struct sockaddr* addr, socklen_t addr_len);


int getpeername(int socket, struct sockaddr* addr, socklen_t* addr_len);
int getsockname(int socket, struct sockaddr* addr, socklen_t* addr_len);

int setsockopt(int socket, int level, int opt_name, const void* opt_value, socklen_t opt_len);
int getsockopt(int socket, int level, int opt_name, void* opt_value, socklen_t* opt_len);


ssize_t recvmsg(int socket, struct msghdr* message, int flags);
ssize_t recv(int socket, void* buffer, size_t length, int flags);
ssize_t recvfrom(int socket, void* buffer, size_t length, int flags, struct sockaddr* addr, socklen_t* addr_len);

ssize_t sendmsg(int socket, const struct msghdr* message, int flags);
ssize_t send(int socket, const void* buffer, size_t length, int flags);
ssize_t sendto(int socket, const void* buffer, size_t length, int flags, const struct sockaddr* addr, socklen_t addr_len);

int shutdown(int socket, int how);
int sockatmark(int socket);








__END_DECLS


#endif
