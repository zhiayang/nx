// in.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#ifndef __in_h
#define __in_h

#include <sys/socket.h>

__BEGIN_DECLS

#ifndef __in_port_t_defined
#define __in_port_t_defined
typedef uint16_t in_port_t;
#endif

#ifndef __in_addr_t_defined
#define __in_addr_t_defined
typedef uint32_t in_addr_t;
#endif

#ifndef __sa_family_t_defined
#define __sa_family_t_defined
typedef unsigned short int sa_family_t;
#endif

#ifndef __socklen_t_defined
#define __socklen_t_defined
typedef __socklen_t socklen_t;
#endif

struct in_addr
{
	in_addr_t s_addr;
};

struct sockaddr_in
{
	sa_family_t sin_family;
	in_port_t sin_port;
	struct in_addr sin_addr;
	char sin_zero[8];
};


/* #define SOL_SOCKET 0 - in <sys/socket.h> */
#define IPPROTO_ICMP		1
#define IPPROTO_IP			2
#define IPPROTO_IPV6		3
#define IPPROTO_RAW			4
#define IPPROTO_TCP			5
#define IPPROTO_UDP			6

#define INADDR_ANY			((in_addr_t) 0x00000000)
#define INADDR_BROADCAST	((in_addr_t) 0xffffffff)
#define INADDR_NONE			((in_addr_t) 0xffffffff)

#define INET_ADDRSTRLEN		16
#define INET6_ADDRSTRLEN	46

#define htons(x)			__htobe16(x)
#define ntohs(x)			__be16toh(x)
#define htonl(x)			__htobe32(x)
#define ntohl(x)			__be32toh(x)

#define IPV6_JOIN_GROUP		0
#define IPV6_LEAVE_GROUP	1
#define IPV6_MULTICAST_HOPS	2
#define IPV6_MULTICAST_IF	3
#define IPV6_MULTICAST_LOOP	4
#define IPV6_UNICAST_HOPS	5
#define IPV6_V6ONLY			6

/* TODO:
IN6_IS_ADDR_UNSPECIFIED
IN6_IS_ADDR_LOOPBACK
IN6_IS_ADDR_MULTICAST
IN6_IS_ADDR_LINKLOCAL
IN6_IS_ADDR_SITELOCAL
IN6_IS_ADDR_V4MAPPED
IN6_IS_ADDR_V4COMPAT
IN6_IS_ADDR_MC_NODELOCAL
IN6_IS_ADDR_MC_LINKLOCAL
IN6_IS_ADDR_MC_SITELOCAL
IN6_IS_ADDR_MC_ORGLOCAL
IN6_IS_ADDR_MC_GLOBAL
*/

# define IN6_ARE_ADDR_EQUAL(a,b) \
	((((__const uint32_t *) (a))[0] == ((__const uint32_t *) (b))[0])     \
	 && (((__const uint32_t *) (a))[1] == ((__const uint32_t *) (b))[1])  \
	 && (((__const uint32_t *) (a))[2] == ((__const uint32_t *) (b))[2])  \
	 && (((__const uint32_t *) (a))[3] == ((__const uint32_t *) (b))[3]))

__END_DECLS

#endif
