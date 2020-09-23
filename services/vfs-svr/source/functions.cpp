// functions.cpp
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.

#include <assert.h>

#include <map>
#include <tuple>
#include <optional>

#include <nx/rpc.h>

#include "internal.h"
#include "export/vfs.h"


using namespace nx;

namespace vfs
{
	struct ProcessState
	{
		pid_t pid;
		nx::ipc::mem_ticket_t registeredTicket;


		/*
			TODO: this does not re-use file descriptors, which breaks POSIX compatibility.
			spec:

			All functions that open one or more file descriptors shall, unless specified otherwise,
			atomically allocate the lowest numbered available (that is, not already open in the calling process)
			file descriptor at the time of each allocation.
		*/

		uint64_t nextHandleId = 0;
		std::map<uint64_t, vnode*> openHandles;
	};

	template <typename T>
	static T& extract(nx::rpc::message_t& msg)
	{
		return *reinterpret_cast<T*>(&msg.bytes[0]);
	}

	static std::map<pid_t, ProcessState> processTable;
	static std::map<uint64_t, pid_t> connectionTable;
	static std::map<uint64_t, rpc::Server> serverTable;

	void handleCall(uint64_t conn, nx::rpc::message_t msg)
	{
		auto pst = &processTable[msg.counterpart];
		pst->pid = msg.counterpart;

		connectionTable[conn] = pst->pid;

		rpc::Server* srv = nullptr;
		if(auto it = serverTable.find(conn); it == serverTable.end())
			srv = &serverTable.emplace(conn, rpc::Server(conn)).first->second;

		else
			srv = &it->second;

		assert(srv != nullptr);

		auto op = msg.function;
		if(op == fns::OP_REGISTER)
		{
			fns::registerProc(pst, srv, extract<fns::FnRegister>(msg));
		}
		else if(op == fns::OP_DEREGISTER)
		{
			fns::unregisterProc(pst, srv, extract<fns::FnDeregister>(msg));
		}
		else if(op == fns::OP_BIND)
		{
			fns::bind(pst, srv, extract<fns::FnBind>(msg));
		}
		else if(op == fns::OP_OPEN)
		{
			fns::open(pst, srv, extract<fns::FnOpen>(msg));
		}
		else if(op == fns::OP_CLOSE)
		{
			fns::close(pst, srv, extract<fns::FnClose>(msg));
		}
		else if(op == fns::OP_READ)
		{
			fns::read(pst, srv, extract<fns::FnRead>(msg));
		}
		else if(op == fns::OP_WRITE)
		{
			fns::write(pst, srv, extract<fns::FnWrite>(msg));
		}
		else
		{
			error("invalid op %d from pid %lu", op, pst->pid);
		}
	}








	namespace fns
	{
		static std::optional<std::tuple<void*, size_t, ipc::mem_ticket_t, bool>>
		validate_buffer(ProcessState* pst, const Buffer& buffer)
		{
			auto tik_id = buffer.memTicketId;

			auto check_bounds = [&](ipc::mem_ticket_t ticket, bool release)
				-> std::optional<std::tuple<void*, size_t, ipc::mem_ticket_t, bool>>
			{
				if(buffer.offset > ticket.len || buffer.offset + buffer.length > ticket.len)
				{
					error("pid %lu passed invalid buffer { %lu, %zu, %zu }",
						pst->pid, tik_id, buffer.offset, buffer.length);

					return std::nullopt;
				}
				else
				{
					return std::make_tuple(((uint8_t*) ticket.ptr + buffer.offset), buffer.length, ticket, release);
				}
			};

			if(auto x = pst->registeredTicket.ticketId; x != 0 && x == tik_id)
				return check_bounds(pst->registeredTicket, /* release: */ false);

			auto existing = ipc::find_existing_memory_ticket(tik_id);
			if(existing.ptr != nullptr)
			{
				return check_bounds(existing, /* release: */ false);
			}
			else
			{
				auto mt = ipc::collect_memory_ticket(tik_id);
				if(mt.ptr == nullptr)
				{
					error("invalid memticket %lu from proc %lu", tik_id, pst->pid);
					return std::nullopt;
				}

				return check_bounds(mt, /* release: */ true);
			}
		}



		void registerProc(ProcessState* pst, rpc::Server* srv, FnRegister msg)
		{
			if(pst->registeredTicket.ptr != 0)
				ipc::release_memory_ticket(pst->registeredTicket);

			pst->registeredTicket = ipc::collect_memory_ticket(msg.ticket);
			if(pst->registeredTicket.ptr == 0)
				error("failed to collect ticket %lu for proc %lu", msg.ticket, pst->pid);

			log("proc %lu registered with memticket %lu", pst->pid, pst->registeredTicket);
		}

		void unregisterProc(ProcessState* pst, rpc::Server* srv, FnDeregister msg)
		{
			if(pst->registeredTicket.ptr == 0)
				return;

			ipc::release_memory_ticket(pst->registeredTicket);
			log("proc %lu unregistered memticket %lu", pst->pid, pst->registeredTicket);
		}

		void bind(ProcessState* pst, rpc::Server* srv, FnBind msg)
		{
			srv->error<FnBind>(ERR_NOT_IMPLEMENTED);
		}

		void open(ProcessState* pst, rpc::Server* srv, FnOpen msg)
		{
			auto _buf = validate_buffer(pst, msg.path);
			if(!_buf)
			{
				srv->error<FnOpen>(ERR_INVALID_BUFFER);
				return;
			}

			auto [ buf, len, memticket, needs_release ] = *_buf;

			auto hid = pst->nextHandleId++;
			auto handle = Handle {
				.id     = hid,
				.owner  = pst->pid
			};

			log("proc %lu: open %.*s -> handle(%lu)", pst->pid, (int) len, buf, handle.id);

			if(needs_release)
				ipc::release_memory_ticket(memticket);

			srv->reply<FnOpen>(handle);
		}

		void close(ProcessState* pst, rpc::Server* srv, FnClose msg)
		{
		}

		void read(ProcessState* pst, rpc::Server* srv, FnRead msg)
		{
		}

		void write(ProcessState* pst, rpc::Server* srv, FnWrite msg)
		{
		}
	}
}
