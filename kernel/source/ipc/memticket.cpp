// memticket.cpp
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.


#include "nx.h"
#include "mm.h"
#include "export/ipc_message.h"

namespace nx::ipc
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
		MemoryTicket(uint64_t id, uint64_t flags, size_t userLen, size_t numPages) : id(id), flags(flags), userLen(userLen), numPages(numPages), refcount(0) { }

		MemoryTicket(MemoryTicket&&) = default;

		MemoryTicket(const MemoryTicket&) = delete;
		MemoryTicket& operator = (MemoryTicket&&) = delete;
		MemoryTicket& operator = (const MemoryTicket&) = delete;

		uint64_t id;
		uint64_t flags;

		size_t userLen;
		size_t numPages;
		size_t refcount;

		// note that we don't keep our own mutex, rather we use the mutex of the physical page region.
		vmm::SharedVMRegion::SharedPhysRegion physicalPages;
		nx::bucket_hashmap<scheduler::Process*, nx::bucket_hashmap<VirtAddr, vmm::SharedVMRegion, vmm::PageHasher>> collectors;

		// the id is guaranteed to be monotonically increasing, so we can just use that as the hash.
		size_t hash() const { return id; }
	};

	//? store this somewhere else?
	static uint64_t ticketIdCounter = 0;
	static Synchronised<nx::bucket_hashmap<uint64_t, MemoryTicket>, nx::mutex> tickets;

	uint64_t createMemticket(size_t len, uint64_t flags)
	{
		if(len > 0)
		{
			auto pages = (len + PAGE_SIZE - 1) / PAGE_SIZE;

			if(flags == 0)
				flags = ipc::MEMTICKET_FLAG_READ | ipc::MEMTICKET_FLAG_WRITE;

			auto id = __atomic_add_fetch(&ticketIdCounter, 1, __ATOMIC_SEQ_CST);
			tickets.lock()->insert(id, MemoryTicket(id, flags, len, pages));

			log("memticket", "pid %lu created memticket (%lu, %zu)", scheduler::getCurrentProcess()->processId, id, len);
			return id;
		}

		return -1;
	}

	mem_ticket_t collectMemticket(uint64_t ticketId)
	{
		auto ret_ticket = ipc::mem_ticket_t();
		memset(&ret_ticket, 0, sizeof(ipc::mem_ticket_t));

		tickets.perform([&](auto& tkts) {

			auto it = tkts.find(ticketId);
			if(it == tkts.end())
			{
				warn("memticket", "process %lu tried to collect nonexistent ticket %lu",
					scheduler::getCurrentProcess()->processId, ticketId);

				return;
			}

			auto tik = &it->value;

			LockedSection(&tik->physicalPages.mtx, [&]() {
				auto proc = scheduler::getCurrentProcess();

				// TODO: we might want to abstract this out a bit more?
				auto virt = vmm::allocateAddrSpace(tik->numPages, vmm::AddressSpaceType::User);
				auto svmr = vmm::SharedVMRegion(virt, tik->numPages, &tik->physicalPages);

				proc->addrspace.lock()->addSharedRegion(svmr.clone());

				// mapLazy will not even map the pages present, so both a read and a write will trap
				vmm::mapLazy(virt, tik->numPages,
					vmm::PAGE_USER | ((tik->flags & ipc::MEMTICKET_FLAG_WRITE) ? vmm::PAGE_WRITE : 0), proc);

				tik->refcount++;
				tik->collectors[proc].insert(virt, krt::move(svmr));

				ret_ticket.ptr = virt.ptr();
				ret_ticket.len = tik->userLen;
				ret_ticket.ticketId = tik->id;

				dbg("memticket", "pid %lu collected memticket (%lu, %p -> %p)", scheduler::getCurrentProcess()->processId, tik->id,
					ret_ticket.ptr, (addr_t) ret_ticket.ptr + ret_ticket.len);
			});
		});

		return ret_ticket;
	}

	mem_ticket_t findExistingMemticket(uint64_t ticketId)
	{
		auto found = tickets.map([&](auto& tkts) -> nx::optional<mem_ticket_t> {

			auto fail = opt::some<mem_ticket_t>({ });

			auto tit = tkts.find(ticketId);
			if(tit == tkts.end())
				return opt::none;

			auto tik = &tit->value;
			auto proc = scheduler::getCurrentProcess();

			// check whether the process collected it at all:
			auto it = tik->collectors.find(proc);
			if(it == tik->collectors.end())
				return fail;

			// see if anybody collected it:
			auto it2 = it->value.begin();
			if(it2 == it->value.end())
				return fail;

			auto& svmr = it2->value;

			auto ret = ipc::mem_ticket_t();
			ret.ptr = svmr.addr.ptr();
			ret.len = tik->userLen;
			ret.ticketId = tik->id;

			return opt::some<mem_ticket_t>(ret);
		});

		if(found.empty())
		{
			warn("memticket", "process %lu tried to collect nonexistent ticket %lu",
				scheduler::getCurrentProcess()->processId, ticketId);

			return { };
		}
		else
		{
			return found.get();
		}
	}

	// the reason that we need the user to pass in the entire ticket instead of just the ticket id is because
	// you can collect a ticket more than once (eg to get two separate regions to the same memory, for whatever reason).
	void releaseMemticket(const mem_ticket_t& ticket)
	{
		auto [ cleanup, tktid ] = tickets.map([&](auto& tkts) -> krt::pair<bool, uint64_t> {

			auto it = tkts.find(ticket.ticketId);
			if(it == tkts.end())
			{
				warn("memticket", "process %lu tried to release nonexistent ticket %lu",
					scheduler::getCurrentProcess()->processId, ticket.ticketId);

				return { false, 0 };
			}

			auto tik = &it->value;
			return LockedSection(&tik->physicalPages.mtx, [&]() -> krt::pair<bool, uint64_t> {

				auto proc = scheduler::getCurrentProcess();

				// check if this process collected it at all
				auto it = tik->collectors.find(proc);
				if(it == tik->collectors.end())
					return { false, 0 };

				// find the matching ticket
				auto it2 = it->value.find(VirtAddr(ticket.ptr));
				if(it2 == it->value.end())
					return { false, 0 };

				// delete it
				auto svmr = krt::move(it2->value);
				it->value.erase(it2);

				// if this process released all its collections for *this ticket*, then remove it
				if(it->value.empty())
					tik->collectors.erase(it);

				tik->refcount--;

				// remove the collected ticket from the address space
				vmm::deallocateAddrSpace(svmr.addr, svmr.numPages, proc);
				vmm::unmapAddress(svmr.addr, svmr.numPages, /* ignore: */ true, proc);

				proc->addrspace.lock()->removeSharedRegion(krt::move(svmr));

				dbg("memticket", "pid %lu released ticket (%lu, %p -> %p)", scheduler::getCurrentProcess()->processId, tik->id,
					ticket.ptr, (addr_t) ticket.ptr + ticket.len);

				if(tik->refcount == 0)
				{
					// destroy while we have the lock.
					tik->physicalPages.destroy();
					return { true, tik->id };
				}

				return { false, 0 };
			});
		});

		if(cleanup)
		{
			log("memticket", "cleaning up ticket %lu", tktid);
			tickets.lock()->remove(tktid);
		}
	}
}







