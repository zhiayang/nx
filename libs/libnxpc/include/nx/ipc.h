// ipc.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include <stdint.h>

#include <nx/ipc_message.h>

namespace nx {
namespace ipc
{
	int send(message_t* msg);
	bool poll(uint64_t classFilter, uint64_t senderFilter);

	void receive(message_t* msg, uint64_t classFilter, uint64_t senderFilter);
}
}
