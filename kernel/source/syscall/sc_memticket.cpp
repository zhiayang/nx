// sc_memticket.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "mm.h"
#include "export/ipc_message.h"

namespace nx
{
	uint64_t syscall::create_memory_ticket(size_t len, uint64_t flags)
	{
		if(len == 0)
			return -1;

		return ipc::createMemticket(len, flags);
	}

	void syscall::collect_memory_ticket(ipc::mem_ticket_t* user_ticket, uint64_t ticketId)
	{
		if(ticketId == 0)
			return;

		auto ret_ticket = ipc::collectMemticket(ticketId);
		if(ret_ticket.ticketId == 0)
			return;

		if(!copy_to_user(user_ticket, &ret_ticket, sizeof(ipc::mem_ticket_t)))
			return;
	}

	void syscall::find_existing_memory_ticket(ipc::mem_ticket_t* user_ticket, uint64_t ticketId)
	{
		if(ticketId == 0)
			return;

		auto ret_ticket = ipc::findExistingMemticket(ticketId);
		if(ret_ticket.ticketId == 0)
			return;

		if(!copy_to_user(user_ticket, &ret_ticket, sizeof(ipc::mem_ticket_t)))
			return;
	}


	void syscall::release_memory_ticket(const ipc::mem_ticket_t* user_ticket)
	{
		auto ticket = ipc::mem_ticket_t();
		if(!copy_to_kernel(&ticket, user_ticket, sizeof(ipc::mem_ticket_t)))
			return;

		ipc::releaseMemticket(ticket);
	}
}
