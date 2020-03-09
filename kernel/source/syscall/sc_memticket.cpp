// sc_memticket.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "export/ipc_message.h"

namespace nx {
namespace syscall
{
	void sc_get_memory_ticket(ipc::mem_ticket_t* ticket, size_t len)
	{
		auto ret = ipc::mem_ticket_t();

		if(len > 0)
		{
			auto pages = (len + PAGE_SIZE - 1) / PAGE_SIZE;


		}

		copy_to_user(&ret, ticket, sizeof(ipc::mem_ticket_t));
	}

	void sc_collect_memory(ipc::mem_ticket_t* ticket, uint64_t ticketId)
	{

	}
}
}
