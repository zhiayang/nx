// Functions.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "../../../include/errno.h"
#include "../../../include/signal.h"
#include "../../../include/orionx/syscall.h"

namespace Library {
namespace SystemCall
{
	/*
		// misc things, page 0+
		.quad	ExitProc			// 0000
		.quad	InstallIRQ			// 0001
		.quad	InstallIRQNoRegs	// 0002

		// process related things, page 4000+
		.quad	CreateThread		// 4000
		.quad	SpawnProcess		// 4001
		.quad	SendSignalToProcess	// 4002
		.quad	SendSignalToThread	// 4003
		.quad	SendMessage			// 4004
		.quad	ReceiveMessage		// 4005
		.quad	Sleep				// 4006
		.quad	Yield				// 4007
		.quad	Block				// 4008
		.quad	InstallSigHandler	// 4009
		.quad	GetPID				// 4010
		.quad	GetParentPID		// 4011
		.quad	ExitThread			// 4012
		.quad	JoinThread			// 4013
		.quad	GetTID				// 4014
		.quad	CreateMutex			// 4015
		.quad	DestroyMutex		// 4016
		.quad	LockMutex			// 4017
		.quad	UnlockMutex			// 4018
		.quad	TryLockMutex		// 4019
		.quad	ForkProcess			// 4020


		// file io things, page 8000+
		.quad	OpenFile			// 8000
		.quad	OpenSocket			// 8001
		.quad	OpenAnyFD			// 8002
		.quad	CloseAnyFD			// 8003
		.quad	ReadAnyFD			// 8004
		.quad	WriteAnyFD			// 8005
		.quad	MemoryMapAnonymous	// 8006
		.quad	MemoryMapFile		// 8007
		.quad	FlushAnyFD			// 8008
		.quad	SeekAnyFD			// 8009
		.quad	StatAnyFD			// 8010
		.quad	GetSeekPos			// 8011
		.quad	BindNetSocket		// 8012
		.quad	ConnectNetSocket	// 8013
		.quad	BindIPCSocket		// 8014
		.quad	ConnectIPCSocket	// 8015
	*/


	void ExitProc()
	{
		Syscall0Param(0);
	}

	void InstallIRQHandler(uint64_t irq, uint64_t handleraddr)
	{
		Syscall2Param(irq, handleraddr, 1);
	}

	void InstallIRQHandlerWithRegs(uint64_t irq, uint64_t handleraddr)
	{
		Syscall2Param(irq, handleraddr, 2);
	}



	pthread_t CreateThread(pthread_attr_t* attr, void (*thr)())
	{
		return Syscall2Param((uintptr_t) thr, (uintptr_t) attr, 4000);
	}

	pid_t SpawnProcess(const char* path, const char* name)
	{
		return Syscall2Param((uint64_t) path, (uint64_t) name, 4001);
	}

	void SignalProcess(pid_t pid, int signum)
	{
		Syscall2Param(pid, signum, 4002);
	}

	void SignalThread(pid_t tid, int signum)
	{
		Syscall2Param(tid, signum, 4003);
	}

	int SendMessage(const char* path, void* msg, size_t size, uint64_t flags)
	{
		return (int) Syscall4Param((uintptr_t) path, (uint64_t) msg, size, flags, 4004);
	}

	ssize_t ReceiveMessage(const char* path, void* msg, size_t size, uint64_t type, uint64_t flags)
	{
		return (int) Syscall5Param((uintptr_t) path, (uint64_t) msg, size, type, flags, 4005);
	}

	void Sleep(uint64_t ms)
	{
		Syscall1Param(ms, 4006);
	}

	void Yield()
	{
		Syscall0Param(4007);
	}

	void Block()
	{
		Syscall0Param(4008);
	}

	sighandler_t InstallSignalHandler(uint64_t signum, sighandler_t handler)
	{
		return (sighandler_t) Syscall2Param(signum, (uint64_t) handler, 4009);
	}

	uint64_t GetPID()
	{
		return Syscall0Param(4010);
	}

	uint64_t GetParentPID()
	{
		return Syscall0Param(4011);
	}

	void ExitThread()
	{
		Syscall0Param(4012);
	}

	void* JoinThread(uint64_t tid)
	{
		return (void*) Syscall1Param(tid, 4013);
	}

	pthread_t GetTID()
	{
		return (pthread_t) Syscall0Param(4014);
	}


	void CreateMutex(pthread_mutex_t* mtx, const pthread_mutexattr_t* attr)
	{
		Syscall2Param((uintptr_t) mtx, (uintptr_t) attr, 4015);
	}

	void DestroyMutex(pthread_mutex_t* mtx)
	{
		Syscall1Param((uintptr_t) mtx, 4016);
	}

	int64_t LockMutex(pthread_mutex_t* mtx)
	{
		return Syscall1Param((uintptr_t) mtx, 4017);
	}

	int64_t UnlockMutex(pthread_mutex_t* mtx)
	{
		return Syscall1Param((uintptr_t) mtx, 4018);
	}

	int64_t TryLockMutex(pthread_mutex_t* mtx)
	{
		return Syscall1Param((uintptr_t) mtx, 4019);
	}

	int64_t ForkProcess()
	{
		return Syscall0Param(4020);
	}


















	// 8000

	uint64_t OpenSocket(uint64_t domain, uint64_t type, uint64_t protocol)
	{
		return Syscall3Param(domain, type, protocol, 8001);
	}

	uint64_t Open(const char* path, uint64_t flags)
	{
		return Syscall2Param((uint64_t) path, flags, 8002);
	}

	void Close(uint64_t fd)
	{
		Syscall1Param(fd, 8003);
	}

	uint64_t Read(uint64_t sd, void* buffer, uint64_t length)
	{
		return Syscall3Param(sd, (uint64_t) buffer, length, 8004);
	}

	uint64_t Write(uint64_t sd, const void* buffer, uint64_t length)
	{
		return Syscall3Param(sd, (uint64_t) buffer, length, 8005);
	}

	uint64_t MMap_Anonymous(uint64_t addr, uint64_t size, uint64_t prot, uint64_t flags)
	{
		return Syscall4Param(addr, size, prot, flags, 8006);
	}

	// 8007, mmap file

	int Flush(uint64_t fd)
	{
		return (int) Syscall1Param(fd, 8008);
	}

	int Seek(uint64_t fd, off_t offset, int whence)
	{
		return (int) Syscall3Param(fd, offset, whence, 8009);
	}

	int Stat(uint64_t fd, struct stat* st, bool statlink)
	{
		return (int) Syscall3Param(fd, (uintptr_t) st, statlink, 8010);
	}

	uint64_t GetSeekPos(uint64_t fd)
	{
		return Syscall1Param(fd, 8011);
	}

	uint64_t BindNetSocket(uint64_t fd, uint32_t ipv4addr, uint16_t port)
	{
		return Syscall3Param(fd, ipv4addr, port, 8012);
	}

	uint64_t ConnectNetSocket(uint64_t fd, uint32_t ipv4addr, uint16_t port)
	{
		return Syscall3Param(fd, ipv4addr, port, 8013);
	}

	uint64_t BindIPCSocket(uint64_t fd, const char* path)
	{
		return Syscall2Param(fd, (uint64_t) path, 8014);
	}

	uint64_t ConnectIPCSocket(uint64_t fd, const char* path)
	{
		return Syscall2Param(fd, (uint64_t) path, 8015);
	}

}
}






extern "C" void* __fetch_errno()
{
	return &errno;
}














