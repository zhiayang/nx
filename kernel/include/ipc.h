// ipc.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once
#include "defs.h"

#include "../../../libs/libnxpc/include/nx/ipc_message.h"

namespace nx
{
	namespace ipc
	{
		void init();

		void addMessage(message_t* message);
	}
}










