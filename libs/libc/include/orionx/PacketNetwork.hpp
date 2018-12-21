// PacketNetwork.hpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.



#include <stdint.h>
#pragma once


namespace Library
{
	union IPv4Address
	{
		uint32_t raw;
		uint8_t bytes[4];

		struct
		{
			uint8_t b1;
			uint8_t b2;
			uint8_t b3;
			uint8_t b4;

		} __attribute__ ((packed));

		explicit IPv4Address() { this->raw = 0; }
		IPv4Address(uint32_t r) { this->raw = __builtin_bswap32(r); }

		operator uint32_t () const
		{
			return this->raw;
		}

		bool operator==(IPv4Address other) const
		{
			return this->raw == other.raw;
		}

	} __attribute__ ((packed));

	union IPv6Address
	{
		uint8_t bytes[16];
		struct
		{
			uint64_t high;
			uint64_t low;
		};

	} __attribute__ ((packed));


	struct IPv4PortAddress
	{
		IPv4Address ip;
		uint16_t port;
	};

	struct IPv6PortAddress
	{
		IPv6Address ip;
		uint16_t port;
	};


	enum class SocketProtocol
	{
		Invalid,
		TCP,
		UDP,
		RawIPv4,
		RawIPv6,

		IPC
	};
}

