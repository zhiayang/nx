// tty.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "tty.h"

//* note: we do not link with libkrt! we only want the template types, which are
//* all in headers.

#include <krt.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct allocator
{
	static void* alloc(size_t len, size_t align)
	{
		return memalign(align, len);
	}

	static void deallocate(void* ptr)
	{
		return free(ptr);
	}
};

struct aborter
{
	static void abort(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);

		fprintf(stderr, "abort: ");
		vfprintf(stderr, fmt, args);

		va_end(args);

		::abort();
	}

	static void debuglog(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);

		fprintf(stderr, "debuglog: ");
		vfprintf(stderr, fmt, args);

		va_end(args);
	}
};


template <typename T>
using list = krt::list<T, allocator, aborter>;

namespace ttysvr
{
	struct tty_t
	{
		uint16_t id;                // the id of the tty. cannot be 0.
		uint16_t flags;

		uint16_t numCols;           // x-dim of the tty
		uint16_t numRows;           // y-dim of the tty

		uint16_t cursorX;
		uint16_t cursorY;

		char* outputInternalBuffer = 0;
		size_t outputInternalBufSz = 0;

		char* outputBuffer = 0;
		size_t outputBufSz = 0;

		char* inputInternalBuffer = 0;
		size_t inputInternalBufSz = 0;

		char* inputBuffer = 0;
		size_t inputBufSz = 0;
	};




	list<tty_t> ttyList;
	tty_t* currentTTY = 0;

	void switchToTTY(uint16_t id)
	{
		for(auto it = ttyList.begin(); it != ttyList.end(); it++)
		{
			if(it->id == id)
			{
				currentTTY = &*it;
				break;
			}
		}

		// wtf man
		// ignore it.
	}

	uint16_t getCurrentTTY()
	{
		assert(currentTTY);
		return currentTTY->id;
	}

	size_t readFromTTY(uint16_t tty, void* buf, size_t len)
	{
		return 0;
	}

	size_t writeToTTY(uint16_t tty, void* buf, size_t len)
	{
		return 0;
	}

	void flushTTY(uint16_t tty)
	{
	}

	uint16_t createTTY()
	{
		return 0;
	}

	void destroyTTY(uint16_t tty)
	{
	}
}































