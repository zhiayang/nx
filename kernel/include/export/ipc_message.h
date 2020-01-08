// ipc_message.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include <stdint.h>

namespace nx {
namespace ipc
{
	struct message_body_t
	{
		union {
			struct {
				uint64_t a;
				uint64_t b;
				uint64_t c;
				uint64_t d;
			};

			uint64_t quads[4];
			uint8_t bytes[32];
		};
	};

	struct signal_message_body_t
	{
		uint64_t sigType;
		union {
			struct {
				uint64_t a;
				uint64_t b;
				uint64_t c;
			};

			uint64_t quads[3];
			uint8_t bytes[24];
		};
	};

	static_assert(sizeof(uint64_t) == 8);
	static_assert(sizeof(uint8_t) * 8 == sizeof(uint64_t));
}
}










