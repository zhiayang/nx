// ipc_message.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include <stdint.h>

namespace nx {
namespace ipc
{
	constexpr uint32_t MAGIC_LE         = 0x6370786e;   // 'nxpc' but in little endian so 'cpxn'
	constexpr uint32_t MAGIC_BE         = 0x6e787063;   // 'nxpc'

	// should be respectable enough. anything more pls use shared mem
	constexpr size_t MAX_MESSAGE_SIZE   = 0x4000;

	constexpr uint8_t CUR_VERSION       = 1;

	// message classes
	constexpr uint64_t CLASS_EMPTY      = 0;
	constexpr uint64_t CLASS_TTY        = 1;

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
}
}
