// ipc_message.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include <stdint.h>

namespace nx {
namespace ipc
{
	struct selector_t
	{
		const char* name;
	};

	struct message_body_t
	{
		static constexpr size_t BufferSize = 256;

		uint8_t bytes[BufferSize];
	};

	struct signal_message_body_t
	{
		signal_message_body_t() { }
		signal_message_body_t(uint64_t a, uint64_t b, uint64_t c) : a(a), b(b), c(c) { }

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

	constexpr uint64_t SIGNAL_NORMAL        = 0;
	constexpr uint64_t SIGNAL_POSIX         = 1;
	constexpr uint64_t SIGNAL_DEVICE_IRQ    = 2;

	constexpr uint64_t SIGNAL_TERMINATE     = 31;

	// params: senderId, msg->sigType, msg->a, msg->b, msg->c.
	// return: some flags, kernel decides what to do.
	using signal_handler_fn_t = uint64_t (*)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

	constexpr uint64_t SIGNAL_IRQ_HANDLED_FLAG = 0x1'0000'0000;
	constexpr uint64_t SIGNAL_IRQ_IGNORED_FLAG = 0x2'0000'0000;

	constexpr uint64_t SIGNAL_IRQ_HANDLED(uint64_t irq) { return (uint32_t) irq | SIGNAL_IRQ_HANDLED_FLAG; }
	constexpr uint64_t SIGNAL_IRQ_IGNORED(uint64_t irq) { return (uint32_t) irq | SIGNAL_IRQ_IGNORED_FLAG; }




	struct mem_ticket_t
	{
		void* ptr;
		size_t len;

		uint64_t ticketId;
	};
}
}












