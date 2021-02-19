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
	VfsState state;

	struct ProcessState
	{
		pid_t pid;
		nx::ipc::mem_ticket_t registeredTicket;

		// this is a vector so that we can get the posix behaviour of having new
		// file descriptors get the lowest free number.
		std::vector<vnode*> openHandles;
	};

	template <typename T>
	static T& extract(nx::rpc::message_t& msg)
	{
		return *reinterpret_cast<T*>(&msg.bytes[0]);
	}

	// TODO: enforce limits on number of open files
	static size_t get_free_handle(ProcessState* pst, vnode* vn)
	{
		for(size_t i = 0; i < pst->openHandles.size(); i++)
		{
			if(pst->openHandles[i] == nullptr)
			{
				pst->openHandles[i] = vn;
				return i;
			}
		}

		pst->openHandles.push_back(vn);
		return pst->openHandles.size() - 1;
	}

	static std::pair<size_t, vnode*> get_free_vnode()
	{
		for(size_t i = 0; i < state.vnodeTable.size(); i++)
		{
			if(state.vnodeTable[i].inode == 0)
				return { i, &state.vnodeTable[i] };
		}

		state.vnodeTable.emplace_back();
		return { state.vnodeTable.size() - 1, &state.vnodeTable.back() };
	}

	void handleCall(uint64_t conn, nx::rpc::message_t msg)
	{
		auto pst = &state.processTable[msg.counterpart];
		pst->pid = msg.counterpart;

		state.connectionTable[conn] = pst->pid;

		rpc::Server* srv = nullptr;
		if(auto it = state.serverTable.find(conn); it == state.serverTable.end())
			srv = &state.serverTable.emplace(conn, rpc::Server(conn)).first->second;

		else
			srv = &it->second;

		assert(srv != nullptr);

		auto op = msg.function;
		if(op == fns::OP_REGISTER)
		{
			fns::rpc_registerProc(pst, srv, extract<fns::Register>(msg));
		}
		else if(op == fns::OP_DEREGISTER)
		{
			fns::rpc_unregisterProc(pst, srv, extract<fns::Deregister>(msg));
		}
		else if(op == fns::OP_BIND)
		{
			fns::rpc_bind(pst, srv, extract<fns::Bind>(msg));
		}
		else if(op == fns::OP_OPEN)
		{
			fns::rpc_open(pst, srv, extract<fns::Open>(msg));
		}
		else if(op == fns::OP_CLOSE)
		{
			fns::rpc_close(pst, srv, extract<fns::Close>(msg));
		}
		else if(op == fns::OP_READ)
		{
			fns::rpc_read(pst, srv, extract<fns::Read>(msg));
		}
		else if(op == fns::OP_WRITE)
		{
			fns::rpc_write(pst, srv, extract<fns::Write>(msg));
		}
		else if(op == fns::OP_SPAWN_FS)
		{
			fns::rpc_spawn_fs(srv, extract<fns::SpawnFilesystem>(msg));
		}
		else if(op == fns::OP_REGISTER_FS)
		{
			fns::rpc_register_fs(srv, extract<fns::RegisterFilesystem>(msg));
		}
		else if(op == fns::OP_DEREGISTER_FS)
		{
			fns::rpc_deregister_fs(srv, extract<fns::DeregisterFilesystem>(msg));
		}
		else if(op == fns::OP_CONNECT_INITRD)
		{
			fns::rpc_connect_initrd(srv, extract<fns::ConnectInitialRamdisk>(msg));
		}
		else
		{
			error("invalid op {} from pid {}", op, pst->pid);
		}
	}








	namespace fns
	{
		using ReturnTicket = std::tuple<void*, size_t, ipc::mem_ticket_t, bool>;

		static std::optional<ReturnTicket> check_ticket_bounds(const Buffer& buffer, ipc::mem_ticket_t ticket, bool release)
		{
			if(buffer.offset > ticket.len || buffer.offset + buffer.length > ticket.len)
			{
				error("invalid buffer received: (id: {}, ofs: {}, len: {})", buffer.memTicketId, buffer.offset, buffer.length);
				return std::nullopt;
			}
			else
			{
				return std::make_tuple(((uint8_t*) ticket.ptr + buffer.offset), buffer.length, ticket, release);
			}
		}

		std::optional<ReturnTicket> validate_buffer(const Buffer& buffer)
		{
			auto tik_id = buffer.memTicketId;

			auto existing = ipc::find_existing_memory_ticket(tik_id);
			if(existing.ptr != nullptr)
			{
				return check_ticket_bounds(buffer, existing, /* release: */ false);
			}
			else
			{
				auto mt = ipc::collect_memory_ticket(tik_id);
				if(mt.ptr == nullptr)
					return std::nullopt;

				return check_ticket_bounds(buffer, mt, /* release: */ true);
			}
		}

		std::optional<ReturnTicket> validate_buffer(ProcessState* pst, const Buffer& buffer)
		{
			auto tik_id = buffer.memTicketId;

			if(auto x = pst->registeredTicket.ticketId; x != 0 && x == tik_id)
				return check_ticket_bounds(buffer, pst->registeredTicket, /* release: */ false);

			auto ret = validate_buffer(buffer);
			if(!ret)
				error("invalid memticket {} from proc {}", tik_id, pst->pid);

			return ret;
		}



		void rpc_registerProc(ProcessState* pst, rpc::Server* srv, Register msg)
		{
			if(pst->registeredTicket.ptr != 0)
				ipc::release_memory_ticket(pst->registeredTicket);

			pst->registeredTicket = ipc::collect_memory_ticket(msg.ticket);
			if(pst->registeredTicket.ptr == 0)
				error("failed to collect ticket {} for proc {}", msg.ticket, pst->pid);

			log("proc {} registered with memticket {}", pst->pid, pst->registeredTicket.ticketId);

			srv->reply<fns::Register>();
		}

		void rpc_unregisterProc(ProcessState* pst, rpc::Server* srv, Deregister msg)
		{
			if(pst->registeredTicket.ptr == 0)
				return;

			ipc::release_memory_ticket(pst->registeredTicket);
			log("proc {} unregistered memticket {}", pst->pid, pst->registeredTicket.ticketId);

			srv->reply<fns::Deregister>();
		}

		void rpc_bind(ProcessState* pst, rpc::Server* srv, Bind msg)
		{
			auto _buf = validate_buffer(pst, msg.path);
			if(!_buf)
			{
				srv->error<fns::Open>(ERR_INVALID_BUFFER);
				return;
			}

			// copy the path and release the buffer asap.
			auto [ buf, len, memticket, needs_release ] = *_buf;
			auto path = std::string((const char*) buf, len);
			if(needs_release)
				ipc::release_memory_ticket(memticket);


			if(msg.flags & BIND_FILESYSTEM)
			{

			}
			else
			{
				srv->error<fns::Bind>(ERR_NOT_IMPLEMENTED);
				return;
			}
		}

		void rpc_open(ProcessState* pst, rpc::Server* srv, Open msg)
		{
			auto _buf = validate_buffer(pst, msg.path);
			if(!_buf)
			{
				srv->error<fns::Open>(ERR_INVALID_BUFFER);
				return;
			}

			auto [ buf, len, memticket, needs_release ] = *_buf;
			auto path = std::string_view((const char*) buf, len);

			auto fs = getFilesystemForPath(path);
			if(fs == nullptr)
			{
				dbg("proc {}: no filesystem for path {}", path);
				srv->error<fns::Open>(ERR_FILE_NOT_FOUND);
				return;
			}

			auto fs_sel = fs->sel_drv;
			auto dv_sel = fs->sel_dev;





			auto [ vni, vnode ] = get_free_vnode();
			assert(vnode != nullptr);
			assert(vni != 0);

			auto fd = get_free_handle(pst, vnode);
			auto handle = Handle {
				.id    = fd,
				.owner = pst->pid
			};

			dbg("proc {}: open {} -> handle({})", pst->pid, path, handle.id);

			if(needs_release)
				ipc::release_memory_ticket(memticket);

			srv->reply<fns::Open>(handle);
		}

		void rpc_close(ProcessState* pst, rpc::Server* srv, Close msg)
		{
			srv->error<fns::Close>(ERR_NOT_IMPLEMENTED);
		}

		void rpc_read(ProcessState* pst, rpc::Server* srv, Read msg)
		{
			dbg("proc {}: read(handle: {})", pst->pid, msg.handle.id);
			auto _buf = validate_buffer(pst, msg.buffer);
			if(!_buf)
			{
				srv->error<fns::Read>(ERR_INVALID_BUFFER);
				return;
			}

			auto [ buf, len, memticket, needs_release ] = *_buf;




			if(needs_release)
				ipc::release_memory_ticket(memticket);

			srv->error<fns::Read>(ERR_NOT_IMPLEMENTED);
		}

		void rpc_write(ProcessState* pst, rpc::Server* srv, Write msg)
		{
			srv->error<fns::Write>(ERR_NOT_IMPLEMENTED);
		}



		void rpc_spawn_fs(rpc::Server* srv, SpawnFilesystem msg)
		{
			if(msg.fs_type == 0)
			{
				dbg("fs_type autodetection not implemented");
				srv->error<fns::SpawnFilesystem>(ERR_NOT_IMPLEMENTED);
				return;
			}

			srv->error<fns::SpawnFilesystem>(ERR_NOT_IMPLEMENTED);
		}
	}
}
