// placebo.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <ipc.h>
#include <syscall.h>

#include "tty.h"

extern "C" int main()
{
	using namespace nx;

	printf("this is the tty server\n");
	// while(true)
	// {
	// 	size_t bufSz = 0;
	// 	while(!(bufSz = ipc::receive(nullptr, 0)))
	// 		;

	// 	// printf("received a message: %zu\n", bufSz);
	// 	printf("(%zu)", bufSz);

	// 	auto buf = malloc(bufSz);
	// 	auto msgSz = ipc::receive(buf, bufSz);

	// 	if(msgSz > 0)
	// 	{
	// 		auto tty_payload = (ttysvr::payload_t*) buf;
	// 		if(tty_payload->magic != ttysvr::MAGIC)
	// 			continue;

	// 		// TODO: verify the sanity of the tty payload
	// 		syscall::vfs_write(1, tty_payload->data, tty_payload->dataSize);
	// 	}

	// 	free(buf);
	// }
}








