// buffer.h
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <stddef.h>

namespace instrad
{
	struct Buffer
	{
		constexpr Buffer(const uint8_t* xs, size_t len) : bytes(xs), idx(0), len(len) { }

		constexpr uint8_t peek() const  { return this->bytes[this->idx]; }
		constexpr size_t index() const  { return this->idx; }
		constexpr size_t length() const { return this->len; }
		constexpr size_t remaining() const { return this->len - this->idx; }

		constexpr const uint8_t* ptr() const { return &this->bytes[this->idx]; }

		constexpr uint8_t pop()
		{
			if(this->remaining() == 0)
				return 0;

			return this->bytes[this->idx++];
		}
		constexpr bool match(uint8_t b)
		{
			if(this->remaining() == 0)
				return false;

			if(this->bytes[this->idx] == b)
			{
				this->idx++;
				return true;
			}

			return false;
		}

	private:
		const uint8_t* bytes;
		size_t idx;
		size_t len;
	};
}
