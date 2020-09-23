// vfs.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <stddef.h>

#include <nx/ipc.h>

namespace vfs
{
	struct Handle
	{
		uint64_t id;
		pid_t owner;
	};

	struct Buffer
	{
		uint64_t memTicketId;
		size_t offset;
		size_t length;
	};

	/*
		how this interface works: all messages are passed via 'normal' (non-signalling) ipc; there should be
		plenty of space in the message buffer to accommodate all arguments. 'large' arguments are passed via
		Buffers, which is a memticket, and the offset and length of the specific region in the ticket to use.

		this lets us theoretically support arbitrarily large paths and buffers, and we use shared memory to
		efficiently copy any data to read/write to/from the target process without going through the kernel.

		for efficiency reasons, it is recommended that processes perform a FnRegister as part of runtime
		initialisation. essentially, doing so allows the vfs server to keep one memticket in its virtual
		address space throughout the lifetime of the client process. then, when passing Buffer arguments, it
		should refer to the same memticket, with varying offsets and lengths. this negates the overhead from
		having to constantly collect() and release() the ticket when performing IPC calls.

		after FnRegister, the vfs server keeps exactly one reference to the provided ticket, and does not
		release() it until FnDeregister is called. it is good practice, although not strictly necessary, to
		explicitly deregister the client process when it either dies, or no longer needs vfs services.

		when a process dies, the kernel will notify the vfs server and allow it to release() any borrowed
		tickets, to return the physical memory back to the system if necessary.

		note that only one ticket can be registered per-process; any prior ticket registrations are released
		before the new one is collected.


		TODO: vfs-server is vulnerable to denial-of-service attacks if a malicious program tries to exhaust
		its address space by passing large tickets to collect.
	*/

	namespace fns
	{
		constexpr uint64_t OP_REGISTER   = 0;
		constexpr uint64_t OP_DEREGISTER = 1;
		constexpr uint64_t OP_BIND       = 2;
		constexpr uint64_t OP_OPEN       = 3;
		constexpr uint64_t OP_CLOSE      = 4;
		constexpr uint64_t OP_READ       = 5;
		constexpr uint64_t OP_WRITE      = 6;

		constexpr uint64_t ERR_NOT_IMPLEMENTED    = 1;
		constexpr uint64_t ERR_INVALID_BUFFER     = 2;
		constexpr uint64_t ERR_INVALID_ARGS       = 3;

		struct FnRegister
		{
			FnRegister(nx::ipc::memticket_id ticket) : ticket(ticket) { }

			using return_type = void;
			static constexpr auto function_number = OP_REGISTER;


			nx::ipc::memticket_id ticket;
		};

		struct FnDeregister
		{
			FnDeregister(nx::ipc::memticket_id ticket) : ticket(ticket) { }

			using return_type = void;
			static constexpr auto function_number = OP_DEREGISTER;


			nx::ipc::memticket_id ticket;
		};

		struct FnBind
		{
			FnBind(const Handle& source, uint64_t flags, const Buffer& path)
				: source(source), flags(flags), path(path) { }

			using return_type = Handle;
			static constexpr auto function_number = OP_BIND;


			Handle source;
			uint64_t flags;

			Buffer path;
		};

		struct FnOpen
		{
			FnOpen(uint64_t flags, const Buffer& path) : flags(flags), path(path) { }

			using return_type = Handle;
			static constexpr auto function_number = OP_OPEN;


			uint64_t flags;
			Buffer path;
		};

		struct FnClose
		{
			FnClose(const Handle& handle) : handle(handle) { }

			using return_type = void;
			static constexpr auto function_number = OP_CLOSE;


			Handle handle;
		};

		struct FnRead
		{
			FnRead(const Handle& handle, const Buffer& buffer) : handle(handle), buffer(buffer) { }

			using return_type = size_t;
			static constexpr auto function_number = OP_READ;


			Handle handle;
			Buffer buffer;
		};

		struct FnWrite
		{
			FnWrite(const Handle& handle, const Buffer& buffer) : handle(handle), buffer(buffer) { }

			using return_type = size_t;
			static constexpr auto function_number = OP_WRITE;


			Handle handle;
			Buffer buffer;
		};



	}
}
