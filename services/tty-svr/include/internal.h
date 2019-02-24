// internal.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once
#include <stdint.h>
#include <stddef.h>

namespace ttysvr
{
	size_t readFromTTY(uint16_t tty, void* buf, size_t len);
	size_t writeToTTY(uint16_t tty, void* buf, size_t len);

	void flushTTY(uint16_t tty);

	uint16_t createTTY();
	void destroyTTY(uint16_t tty);

	void switchToTTY(uint16_t tty);
	uint16_t getCurrentTTY();
}
