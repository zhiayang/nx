// tarfs.cpp
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdio.h>

#include <utility>

#include <nx/rpc.h>
#include <svr/vfs.h>

int main(int argc, char** argv)
{
	while(true)
	{
		uint64_t conn = 0;
		nx::rpc::message_t msg = { };
		msg = nx::rpc_wait_any_call(&conn);

		// vfs::handleCall(conn, std::move(msg));
	}
}
