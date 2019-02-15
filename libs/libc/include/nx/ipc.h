// ipc.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus

namespace nx {
namespace ipc
{
	constexpr uint32_t MESSAGE_MAGIC_LE     = 0x6370786e;    // 'nxpc' but in little endian so 'cpxn'
	constexpr uint32_t MESSAGE_MAGIC_BE     = 0x6e787063;    // 'nxpc'

	// message classes
	constexpr uint64_t MESSAGE_EMPTY        = 0;

	struct message_t
	{
		uint32_t magic;
		uint32_t version;       // bits 0:7 are the version; bits 8:31 are feature flags.

		uint64_t messageClass;

		uint64_t senderId;      // this should be left empty for the sender; the OS will populate this
		uint64_t targetId;      // -1 (aka 0xffffffffffffffff) for broadcast. do a multicast manually.

		size_t payloadSize;
		uint8_t payload[];
	};

	void send(message_t* msg);
	bool poll(uint64_t classFilter, uint64_t senderFilter);
	void receive(message_t* msg, uint64_t classFilter, uint64_t senderFilter);
}
}

#endif
