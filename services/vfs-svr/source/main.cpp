// main.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdio.h>

#include <map>
#include <vector>
#include <string>

#include "internal.h"
#include "export/vfs.h"

#include <nx/rpc.h>
#include <nx/syscall.h>


int main(int argc, char** argv)
{
	vfs::init();

	while(true)
	{
		uint64_t conn = 0;
		nx::rpc::message_t msg = { };
		msg = nx::rpc_wait_any_call(&conn);

		vfs::handleCall(conn, std::move(msg));
	}
}



























