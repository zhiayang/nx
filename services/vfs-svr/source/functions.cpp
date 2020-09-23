// functions.cpp
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.

#include <map>
#include <tuple>
#include <optional>

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

	static std::map<pid_t, ProcessState> processTable;



	void handleCall(pid_t client, nx::ipc::message_body_t body)
	{
		using namespace msg;

		auto pst = &processTable[client];
		pst->pid = client;

		auto op = ipc::extract<Header>(body).op;

		if(op == OP_REGISTER)
		{
			auto& msg = ipc::extract<FnRegister>(body);
			fns::registerProc(pst, std::move(msg));
		}
		else if(op == OP_DEREGISTER)
		{
			auto& msg = ipc::extract<FnDeregister>(body);
			fns::unregisterProc(pst, std::move(msg));
		}
		else if(op == OP_BIND)
		{
			auto& msg = ipc::extract<FnBind>(body);
			fns::bind(pst, std::move(msg));
		}
		else if(op == OP_OPEN)
		{
			auto& msg = ipc::extract<FnOpen>(body);
			fns::open(pst, std::move(msg));
		}
		else if(op == OP_CLOSE)
		{
			auto& msg = ipc::extract<FnClose>(body);
			fns::close(pst, std::move(msg));
		}
		else if(op == OP_READ)
		{
			auto& msg = ipc::extract<FnRead>(body);
			fns::read(pst, std::move(msg));
		}
		else if(op == OP_WRITE)
		{
			auto& msg = ipc::extract<FnWrite>(body);
			fns::write(pst, std::move(msg));
		}
		else
		{
			error("invalid op %d from pid %lu", op, client);
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



		void registerProc(ProcessState* pst, msg::FnRegister msg)
		{
			if(pst->registeredTicket.ptr != 0)
				ipc::release_memory_ticket(pst->registeredTicket);

			pst->registeredTicket = ipc::collect_memory_ticket(msg.ticket);
			if(pst->registeredTicket.ptr == 0)
				error("failed to collect ticket %lu for proc %lu", msg.ticket, pst->pid);
		}

		void unregisterProc(ProcessState* pst, msg::FnDeregister msg)
		{
			if(pst->registeredTicket.ptr == 0)
				return;

			ipc::release_memory_ticket(pst->registeredTicket);
		}

		void bind(ProcessState* pst, msg::FnBind msg)
		{
			ipc::send(pst->pid, msg::ResBind {
				msg::Header {
					.op         = msg::OP_BIND,
					.status     = msg::STATUS_ERR_NOT_IMPLEMENTED,
					.sequence   = msg.sequence + 1
				},
				Handle {

				}
			});
		}

		void open(ProcessState* pst, msg::FnOpen msg)
		{
			auto _buf = validate_buffer(pst, msg.path);
			if(!_buf)
			{
				ipc::send(pst->pid, msg::ResOpen {
					msg::Header {
						.op         = msg::OP_OPEN,
						.status     = msg::STATUS_ERR_INVALID_BUFFER,
						.sequence   = msg.sequence + 1
					}
				});

				return;
			}

			auto [ buf, len, memticket, needs_release ] = *_buf;

			log("proc %lu: open %.*s", pst->pid, (int) len, buf);

			auto hid = pst->nextHandleId++;
			auto handle = Handle {
				.id     = hid,
				.owner  = pst->pid
			};



			if(needs_release)
				ipc::release_memory_ticket(memticket);

			ipc::send(pst->pid, msg::ResOpen {
				msg::Header {
					.op         = msg::OP_OPEN,
					.status     = msg::STATUS_OK,
					.sequence   = msg.sequence + 1
				},
				handle
			});
		}

		void close(ProcessState* pst, msg::FnClose msg)
		{
		}

		void read(ProcessState* pst, msg::FnRead msg)
		{
		}

		void write(ProcessState* pst, msg::FnWrite msg)
		{
		}
	}
}
