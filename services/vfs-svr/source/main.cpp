// main.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdio.h>

#include <map>
#include <vector>
#include <string>

#include "internal.h"
#include "export/vfs.h"

#include <nx/syscall.h>


int main(int argc, char** argv)
{
	vfs::init();

	while(true)
	{
		if(nx::ipc::poll() == 0)
			continue;

		nx::ipc::message_body_t body;
		auto client = nx::ipc::receive(&body);

		vfs::handleCall(client, std::move(body));
	}
}

















namespace vfs
{
	void vlog(int lvl, const char* fmt, va_list ap)
	{
		char* buf = 0;
		int len = vasprintf(&buf, fmt, ap);

		syscall::kernel_log(lvl, "vfs", 4, buf, len);
		free(buf);
	}

	void dbg(const char* fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		vlog(-1, fmt, ap);
		va_end(ap);
	}

	void log(const char* fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		vlog(0, fmt, ap);
		va_end(ap);
	}

	void warn(const char* fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		vlog(1, fmt, ap);
		va_end(ap);
	}

	void error(const char* fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		vlog(2, fmt, ap);
		va_end(ap);
	}
}
















