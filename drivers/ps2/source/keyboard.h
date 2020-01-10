// keyboard.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdint.h>
#include <algorithm>

#include "krt/types/circbuf.h"

namespace ps2
{
	// copied and stripped from krt. static buffer size.
	template <typename T, size_t BufferSize>
	struct circularbuf
	{
		circularbuf()
		{
			this->cnt = 0;
			memset(this->ptr, 0, BufferSize * sizeof(T));
		}

		~circularbuf() { }
		circularbuf(const circularbuf& other)
		{
			this->cnt       = other.cnt;
			this->readIdx   = other.readIdx;
			this->writeIdx  = other.writeIdx;

			memcpy(this->ptr, other.ptr, BufferSize * sizeof(T));
		}

		// copy assign
		circularbuf& operator = (const circularbuf& other)
		{
			if(this != &other)  return *this = circularbuf(other);
			else                return *this;
		}

		void write(const T& x)
		{
			this->ptr[this->writeIdx] = x;
			this->writeIdx = (this->writeIdx + 1) % BufferSize;

			this->cnt = std::min(this->cnt + 1, BufferSize);
		}

		T read()
		{
			if(this->empty())
				return { };

			auto ret = this->ptr[this->readIdx];
			this->readIdx = (this->readIdx + 1) % BufferSize;

			this->cnt -= 1;
			return ret;
		}

		T peek(size_t i = 0)
		{
			if(this->empty())
				return { };

			return this->ptr[(this->readIdx + i) % BufferSize];
		}

		void write(T* xs, size_t n)
		{
			for(size_t i = 0; i < n; i++)
				this->write(xs[i]);
		}

		void read(T* buf, size_t n)
		{
			const auto num = std::min(n, this->cnt);
			for(size_t i = 0; i < num; i++)
				buf[i] = this->read();
		}

		void consume(size_t n)
		{
			const auto num = std::min(n, this->cnt);
			for(size_t i = 0; i < num; i++)
				this->read();
		}

		void peek(T* buf, size_t n)
		{
			const auto num = std::min(n, this->cnt);
			for(size_t i = 0; i < num; i++)
				buf[i] = this->peek(i);
		}

		void clear()
		{
			this->cnt = 0;
			this->readIdx = 0;
			this->writeIdx = 0;
		}

		T* data()               { return this->ptr; }
		const T* data() const   { return (const T*) this->data; }
		size_t size() const     { return this->cnt; }
		bool empty() const      { return this->cnt == 0; }
		bool full() const       { return this->cnt == BufferSize; }

	private:
		T ptr[BufferSize] = {  };
		size_t readIdx = 0;
		size_t writeIdx = 0;
		size_t cnt = 0;
	};



	struct Keyboard
	{
		void addByte(uint8_t x);

	private:
		uint8_t decodeHID();

		circularbuf<uint8_t, 8> buffer;


		// these are temporary!!
		bool shifting = false;
		void print(uint8_t hid);
	};
}







