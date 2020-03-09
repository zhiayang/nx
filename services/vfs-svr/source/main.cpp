// main.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdio.h>
#include <vector>
#include <string>

#include "internal.h"
#include "export/vfs.h"

#include <syscall.h>

int main()
{
	// vfs::init();

	// while(true)
	// {
	// 	if(nx::ipc::poll() > 0)
	// 	{
	// 		nx::ipc::message_body_t msg;
	// 		nx::ipc::receive(&msg);

	// 		struct omo {
	// 			int a;
	// 			int b;

	// 		} __attribute__((packed));

	// 		auto x = nx::ipc::extract<omo>(msg);
	// 		printf("a = %d, b = %d\n", x.a, x.b);
	// 	}
	// }
	printf("uwu\n");
	printf("bye\n");
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
















