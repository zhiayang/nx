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
		MemoryTicket(uint64_t id, uint64_t flags, size_t numPages) : id(id), flags(flags), numPages(numPages), refcount(0) { }

		MemoryTicket(const MemoryTicket&) = delete;
		MemoryTicket& operator = (const MemoryTicket&) = delete;

		MemoryTicket(MemoryTicket&&) = default;
		MemoryTicket& operator = (MemoryTicket&&) = default;

		uint64_t id;
		uint64_t flags;

		size_t numPages;
		size_t refcount;

		// note that we don't keep our own mutex, rather we use the mutex of the physical page region.
		vmm::SharedVMRegion::SharedPhysRegion physicalPages;
		nx::bucket_hashmap<scheduler::Process*, nx::bucket_hashmap<VirtAddr, vmm::SharedVMRegion, vmm::PageHasher>> collectors;

		// the id is guaranteed to be monotonically increasing, so we can just use that as the hash.
		size_t hash() const { return id; }
	};

	//? store this somewhere else?
	static nx::mutex ticketLock;
	static uint64_t ticketId = 0;
	static nx::bucket_hashmap<uint64_t, MemoryTicket> tickets;

	uint64_t syscall::create_memory_ticket(size_t len, uint64_t flags)
	{
		if(len > 0)
		{
			auto pages = (len + PAGE_SIZE - 1) / PAGE_SIZE;

			if(flags == 0)
				flags = ipc::MEMTICKET_FLAG_READ | ipc::MEMTICKET_FLAG_WRITE;

			return LockedSection(&ticketLock, [&]() -> auto {
				auto id = ++ticketId;

				tickets.insert(id, MemoryTicket(flags, ++ticketId, pages));
				return id;
			});
		}

		return -1;
	}

	void syscall::collect_memory_ticket(ipc::mem_ticket_t* user_ticket, uint64_t ticketId)
	{
		auto ret_ticket = ipc::mem_ticket_t();
		memset(&ret_ticket, 0, sizeof(ipc::mem_ticket_t));

		if(auto it = tickets.find(ticketId); it != tickets.end())
		{
			auto tik = &it->value;

			LockedSection(&tik->physicalPages.mtx, [&]() {
				auto proc = scheduler::getCurrentProcess();

				// TODO: we might want to abstract this out a bit more?
				auto virt = vmm::allocateAddrSpace(tik->numPages, vmm::AddressSpaceType::User);
				auto svmr = vmm::SharedVMRegion(virt, tik->numPages, &tik->physicalPages);

				LockedSection(&proc->addrSpaceLock, [&]() {
					proc->addrspace.addSharedRegion(svmr.clone());

					// vmm::mapLazy(virt, tik->numPages, vmm::PAGE_USER, proc);
				});

				tik->refcount++;
				tik->collectors[proc].insert(virt, krt::move(svmr));

				ret_ticket.ptr = virt.ptr();
				ret_ticket.len = PAGE_SIZE * tik->numPages;
				ret_ticket.ticketId = tik->id;
			});
		}

		if(!copy_to_user(user_ticket, &ret_ticket, sizeof(ipc::mem_ticket_t)))
			return;
	}

	// the reason that we need the user to pass in the entire ticket instead of just the ticket id is because you can collect a ticket more than once
	// (eg to get two separate regions to the same memory, for whatever reason).
	void syscall::release_memory_ticket(const ipc::mem_ticket_t* user_ticket)
	{
		auto ticket = ipc::mem_ticket_t();
		if(!copy_to_kernel(&ticket, user_ticket, sizeof(ipc::mem_ticket_t)))
			return;

		if(auto it = tickets.find(ticketId); it != tickets.end())
		{
			auto tik = &it->value;

			bool cleanup = LockedSection(&tik->physicalPages.mtx, [&]() -> bool {

				auto proc = scheduler::getCurrentProcess();

				// check if this process collected it at all
				auto it = tik->collectors.find(proc);
				if(it == tik->collectors.end())
					return false;

				auto it2 = it->value.find(VirtAddr(ticket.ptr));
				if(it2 == it->value.end())
					return false;

				auto svmr = krt::move(it2->value);
				it->value.erase(it2);

				if(it->value.empty())
					tik->collectors.erase(it);

				tik->refcount--;

				LockedSection(&proc->addrSpaceLock, [&]() {
					proc->addrspace.removeSharedRegion(krt::move(svmr));
				});

				if(tik->refcount == 0)
				{
					// destroy while we have the lock.
					tik->physicalPages.destroy();
					return true;
				}

				return false;
			});

			if(cleanup)
			{
				LockedSection(&ticketLock, [&]() {
					tickets.remove(tik->id);
				});
			}
		}
	}
}
