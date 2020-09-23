// tarfs.cpp
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdio.h>

#include <utility>

#include <nx/ipc.h>
#include <svr/vfs.h>

int main(int argc, char** argv)
{
	while(true)
	{
		if(nx::ipc::poll() == 0)
			continue;

		nx::ipc::message_body_t body;
		auto client = nx::ipc::receive(&body);

		// vfs::handleCall(client, std::move(body));
	}
}
