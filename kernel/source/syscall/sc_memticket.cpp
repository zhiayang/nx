// sc_memticket.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "export/ipc_message.h"

namespace nx {
namespace syscall
{
	/*
		the memticket IPC interface is a mechanism for allocating memory in one process,
		modifying that memory, then using normal IPC to send the ticket ID over to
		other processes that can then collect the memory.

		this is basically the posix shared-memory idea, but not using files on the vfs
		(why does everything use the damn vfs??)

		unlike shm_open, somebody must create the ticket first before it can be used.
		nobody gets to "just" pass O_CREAT to shm_open and perform a "open if existing else create"
		idiom.

		the posix compat layer will probably handle this distinction if we get that far.

		once created, one or more processes can "collect" the memory, which will behave like mmap,
		and map the physical memory of the ticket to its virtual address space, acting like shared
		memory.

		the creator can also specify one or more flags, including read/write access, and exclusivity.

		exclusive memory tickets can only be mapped to one process at a time; if the creator wants
		to allow other processes to collect() the memory, it must first release() it.

		once all processes that called collect() have release()-ed the memory, the backing memory is
		automatically freed.
	*/

	struct MemoryTicket
	{
		uint32_t flags = 0;
		ipc::mem_ticket_t user_ticket;
	};

	void get_memory_ticket(ipc::mem_ticket_t* ticket, size_t len)
	{
		auto ret = ipc::mem_ticket_t();

		if(len > 0)
		{
			auto pages = (len + PAGE_SIZE - 1) / PAGE_SIZE;


		}

		// todo: change these functions to follow memcpy argument order.
		copy_to_user(&ret, ticket, sizeof(ipc::mem_ticket_t));
	}

	void collect_memory_ticket(ipc::mem_ticket_t* ticket, uint64_t ticketId)
	{

	}

	void release_memory_ticket()
	{
	}
}
}
