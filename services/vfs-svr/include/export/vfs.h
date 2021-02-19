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

		TODO: vfs-server doesn't actually get notified when a registered process dies!!!
	*/

	namespace fns
	{
		constexpr uint64_t OP_REGISTER          = 0;
		constexpr uint64_t OP_DEREGISTER        = 1;
		constexpr uint64_t OP_BIND              = 2;
		constexpr uint64_t OP_OPEN              = 3;
		constexpr uint64_t OP_CLOSE             = 4;
		constexpr uint64_t OP_READ              = 5;
		constexpr uint64_t OP_WRITE             = 6;

		constexpr uint64_t OP_CONNECT_INITRD    = 0x100;

		constexpr uint64_t OP_SPAWN_FS          = 0x1000;
		constexpr uint64_t OP_REGISTER_FS       = 0x1001;
		constexpr uint64_t OP_DEREGISTER_FS     = 0x1002;

		constexpr uint64_t ERR_NOT_IMPLEMENTED  = 1;
		constexpr uint64_t ERR_INVALID_BUFFER   = 2;
		constexpr uint64_t ERR_INVALID_ARGS     = 3;
		constexpr uint64_t ERR_FILE_NOT_FOUND   = 4;

		constexpr uint64_t ERR_FS_CODE_EXISTS   = 0x1000;
		constexpr uint64_t ERR_FS_CODE_NO_EXIST = 0x1001;

		constexpr uint64_t BIND_FILESYSTEM      = 0x1;
		constexpr uint64_t BIND_READ_ONLY       = 0x2;

		/**
		 * Register a process that will interact with the VFS server. The calling process should set up the
		 * provided memory ticket with a reasonably sized buffer for communication with the VFS server. Note
		 * that each process can only be registered once; upon registration, any prior registrations are
		 * de-registered, and their corresponding memory tickets released.
		 *
		 * Parameters:
		 * ticket             - the memory ticket to the shared buffer; the VFS server will collect this ticket.
		 *
		 * Return Value:
		 * none
		 *
		 * Error Values:
		 * TODO
		 */
		struct Register
		{
			Register(nx::ipc::memticket_id ticket) : ticket(ticket) { }

			static constexpr auto function_number = OP_REGISTER;
			using return_type = void;
			using error_type = uint64_t;

			nx::ipc::memticket_id ticket;
		};

		/**
		 * Deregister a process that previously registered with the VFS server. Even though the server will
		 * automatically deregister processes that die, it is good practice to call this method before exiting.
		 *
		 * Parameters:
		 * ticket             - the memory ticket that was previously passed to Register.
		 *
		 * Return Value:
		 * none
		 *
		 * Error Values:
		 * TODO
		 */
		struct Deregister
		{
			Deregister(nx::ipc::memticket_id ticket) : ticket(ticket) { }

			static constexpr auto function_number = OP_DEREGISTER;
			using return_type = void;
			using error_type = uint64_t;

			nx::ipc::memticket_id ticket;
		};

		/**
		 * Binds a handle to the given path; it is used to either create directory unions at a given path, or
		 * mount a filesystem at the given path.
		 *
		 * Flags:
		 * BIND_FILESYSTEM    - treat `source.id` as the process id of a filesystem server; the filesystem will then
		 *                      be mounted at the given path.
		 *
		 * BIND_READ_ONLY     - for filesystems, mount it as read-only; for unions, bind the directory as read only.
		 *
		 * Parameters:
		 * source             - the given handle to bind to the path.
		 * flags              - the flags to use.
		 *
		 * Return Value:
		 * A handle to the bound path. If the path was already bound to something, the handle id will be the same.
		 *
		 * Error Values:
		 * TODO
		 */
		struct Bind
		{
			Bind(const Handle& source, uint64_t flags, const Buffer& path)
				: source(source), flags(flags), path(path) { }

			static constexpr auto function_number = OP_BIND;
			using return_type = Handle;
			using error_type = uint64_t;

			Handle source;
			uint64_t flags;

			Buffer path;
		};

		struct Open
		{
			Open(uint64_t flags, const Buffer& path) : flags(flags), path(path) { }

			static constexpr auto function_number = OP_OPEN;
			using return_type = Handle;
			using error_type = uint64_t;

			uint64_t flags;
			Buffer path;
		};

		struct Close
		{
			Close(const Handle& handle) : handle(handle) { }

			static constexpr auto function_number = OP_CLOSE;
			using return_type = void;
			using error_type = uint64_t;

			Handle handle;
		};

		struct Read
		{
			Read(const Handle& handle, const Buffer& buffer) : handle(handle), buffer(buffer) { }

			static constexpr auto function_number = OP_READ;
			using return_type = size_t;
			using error_type = uint64_t;

			Handle handle;
			Buffer buffer;
		};

		struct Write
		{
			Write(const Handle& handle, const Buffer& buffer) : handle(handle), buffer(buffer) { }

			static constexpr auto function_number = OP_WRITE;
			using return_type = size_t;
			using error_type = uint64_t;

			Handle handle;
			Buffer buffer;
		};




		/**
		 * Mount a filesystem for the given device. In most cases, `fs_type` should be left as 0 to autodetect
		 * the correct driver for the filesystem.
		 *
		 * Parameters:
		 * device             - the handle to the device driver
		 * fs_type            - the filesystem identifier, for selecting a specific driver. use 0 to autodetect.
		 *
		 * Return Value:
		 * The pid of the spawned filesystem server; this should be passed to `Bind` to bind it to a real path.
		 *
		 * Error Values:
		 * TODO
		 */
		struct SpawnFilesystem
		{
			SpawnFilesystem(const pid_t& device, uint64_t fs_type) : device(device), fs_type(fs_type) { }

			static constexpr auto function_number = OP_SPAWN_FS;
			using return_type = pid_t;
			using error_type = uint64_t;

			pid_t device;
			uint64_t fs_type;
		};

		/**
		 * Register a filesystem server for the given `fs_type`; each filesystem should have a unique fs_type code.
		 * If the given fs_code is already taken by another driver, then an error is returned. The registration
		 * takes a path to the server (executable) that is spawned for each instance of the filesystem that is created.
		 *
		 * The provided executable does not need to exist at the time of registration; it is only checked when
		 * an attempt is made to spawn the filesystem server.
		 *
		 * Since this is not a process-based transaction, there is no need to 'Register' and 'Deregister' the
		 * memticket before calling this procedure. The server will automatically collect and release the ticket
		 * as appropriate.
		 *
		 * Parameters:
		 * fs_type              - the unique code identifying the filesystem
		 * serverPath           - a buffer containing the path to the filesystem server to spawn
		 *
		 * Return Value:
		 * void
		 *
		 * Error Values:
		 * ERR_FS_CODE_EXISTS   - the given code is already in use
		 */
		struct RegisterFilesystem
		{
			RegisterFilesystem(uint64_t fs_type, const Buffer& serverPath) : fs_type(fs_type), serverPath(serverPath) { }

			static constexpr auto function_number = OP_REGISTER_FS;
			using return_type = void;
			using error_type = uint64_t;

			uint64_t fs_type;
			Buffer serverPath;
		};

		/**
		 * Deregister the filesystem server associated with the given `fs_type`.
		 *
		 * Parameters:
		 * fs_type              - the unique code identifying the filesystem
		 *
		 * Return Value:
		 * void
		 *
		 * Error Values:
		 * ERR_FS_CODE_NO_EXIST - the given code was not registered to a filesystem
		 */
		struct DeregisterFilesystem
		{
			DeregisterFilesystem(uint64_t fs_type) : fs_type(fs_type) { }

			static constexpr auto function_number = OP_DEREGISTER_FS;
			using return_type = void;
			using error_type = uint64_t;

			uint64_t fs_type;
		};


		/**
		 * Connect the initrd to VFS. The ticket's memory does not need to be retained; the VFS
		 * will make a copy of the entire buffer in its own memory.
		 *
		 * Parameters:
		 * initrd               - the buffer containing the initrd
		 *
		 * Return Value:
		 * void
		 *
		 * Error Values:
		 * ERR_INVALID_BUFFER   - the buffer provided was invalid
		 */
		struct ConnectInitialRamdisk
		{
			ConnectInitialRamdisk(const Buffer& initrd) : initrd(initrd) { }

			static constexpr auto function_number = OP_CONNECT_INITRD;
			using return_type = void;
			using error_type = uint64_t;

			Buffer initrd;
		};
	}
}
