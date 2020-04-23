// sc_memticket.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"
#include "mm.h"
#include "export/ipc_message.h"

namespace nx
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
		MemoryTicket(uint64_t id, uint64_t flags, size_t numPages)
			: id(id), flags(flags), numPages(numPages) { }

		uint64_t id;
		uint64_t flags;

		size_t numPages;

		nx::treemap<scheduler::Process*, krt::pair<addr_t, size_t>> collectors;

		size_t hash() const { return krt::hash_combine(id, flags, numPages); }
	};

	//? store this somewhere else?
	static uint64_t ticketId = 0;
	static nx::bucket_hashmap<uint64_t, MemoryTicket> tickets;

	void syscall::create_memory_ticket(ipc::mem_ticket_t* user_ticket, size_t len, uint64_t flags)
	{
		auto ret = ipc::mem_ticket_t();

		if(len > 0)
		{
			auto pages = (len + PAGE_SIZE - 1) / PAGE_SIZE;

			if(flags == 0)
				flags = ipc::MEMTICKET_FLAG_READ | ipc::MEMTICKET_FLAG_WRITE;

			auto ticket = MemoryTicket(flags, ++ticketId, pages);
			tickets.insert(ticket.id, ticket);

			// TODO: we need some method of accounting for physical pages that don't "belong" to a particular
			// process. because nobody really "owns" a memory ticket, it's just that the creator of it
			// happens to be the one "borrowing" it.

			// the current mechanism for lazy allocation tags a physical page to a given virtual page, and that
			// virtual page must belong to a process (or more accurately, to an AddressSpace). so when the
			// process dies, it kills the AddressSpace which frees the physical pages for all the allocated
			// virtual pages.

			// actually that was a lie. nothing like that happens, pages leak like crazy.
		}

		copy_to_user(user_ticket, &ret, sizeof(ipc::mem_ticket_t));
	}

	void syscall::collect_memory_ticket(ipc::mem_ticket_t* ticket, uint64_t ticketId)
	{

	}

	void syscall::release_memory_ticket(ipc::mem_ticket_t* ticket)
	{
	}
}
