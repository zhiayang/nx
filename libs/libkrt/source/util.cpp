// util.cpp
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include "krt.h"

namespace krt {
namespace util
{
	struct ctx_t
	{
		size_t ofs;
		char* buffer;
	};

	size_t humanSizedBytes(char* buffer, size_t _bytes, bool thousand)
	{
		// keep dividing by 1024 (or 1000)
		double bytes = _bytes;
		double divisor = (thousand ? 1000 : 1024);

		int idx = 0;
		while(bytes > divisor && idx < 7)
			bytes /= divisor, idx++;

		auto callback = [](void* c, const char* str, size_t len) -> size_t {
			auto ctx = (ctx_t*) c;

			for(size_t i = 0; i < len; i++)
				ctx->buffer[i + ctx->ofs] = str[i];

			ctx->ofs += len;
			return len;
		};

		// ok now we need to print to the buffer...
		// use vcbprintf.
		ctx_t ctx;
		ctx.ofs = 0;
		ctx.buffer = buffer;

		return cbprintf((void*) &ctx, callback, "%.2f %cB", bytes, "\0KMGTPY"[idx]);
	}
}
}