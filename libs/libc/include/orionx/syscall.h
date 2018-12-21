// syscall.h
// Copyright (c) 2018, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdint.h>
#include <signal.h>
#include <pthread.h>
#include <sys/stat.h>
#pragma once

#ifdef __cplusplus
namespace Library
{
	namespace SystemCall
{
		extern "C" uint64_t Syscall0Param(uint64_t scvec);
		extern "C" uint64_t Syscall1Param(uint64_t p1, uint64_t scvec);
		extern "C" uint64_t Syscall2Param(uint64_t p1, uint64_t p2, uint64_t scvec);
		extern "C" uint64_t Syscall3Param(uint64_t p1, uint64_t p2, uint64_t p3, uint64_t scvec);
		extern "C" uint64_t Syscall4Param(uint64_t p1, uint64_t p2, uint64_t p3, uint64_t p4, uint64_t scvec);
		extern "C" uint64_t Syscall5Param(uint64_t p1, uint64_t p2, uint64_t p3, uint64_t p4, uint64_t p5, uint64_t scvec);


		void ExitProc();
		void InstallIRQHandler(uint64_t irq, uint64_t handleraddr);
		void InstallIRQHandlerWithRegs(uint64_t irq, uint64_t handleraddr);
		pthread_t CreateThread(pthread_attr_t* attribs, void (*thr)());
		pid_t SpawnProcess(const char* path, const char* name);
		void SignalProcess(pid_t pid, int signum);
		void SignalThread(pid_t tid, int signum);
		int SendMessage(const char* path, void* msg, size_t size, uint64_t flags);
		ssize_t ReceiveMessage(const char* path, void* msg, size_t size, uint64_t type, uint64_t flags);
		void Sleep(uint64_t ms);
		void Yield();
		void Block();
		sighandler_t InstallSignalHandler(uint64_t signum, sighandler_t handler);
		uint64_t GetPID();
		uint64_t GetParentPID();
		void ExitThread();
		void* JoinThread(uint64_t tid);
		pthread_t GetTID();

		void CreateMutex(pthread_mutex_t* mtx, const pthread_mutexattr_t* attr);
		void DestroyMutex(pthread_mutex_t* mtx);
		int64_t LockMutex(pthread_mutex_t* mtx);
		int64_t UnlockMutex(pthread_mutex_t* mtx);
		int64_t TryLockMutex(pthread_mutex_t* mtx);
		int64_t ForkProcess();


		uint64_t Open(const char* path, uint64_t flags);
		void Close(uint64_t fd);
		uint64_t Read(uint64_t sd, void* buffer, uint64_t length);
		uint64_t Write(uint64_t sd, const void* buffer, uint64_t length);
		uint64_t MMap_Anonymous(uint64_t addr, uint64_t size, uint64_t prot, uint64_t flags);
		int Flush(uint64_t fd);
		int Seek(uint64_t fd, off_t offset, int whence);
		int Stat(uint64_t fd, struct stat* st, bool statlink);
		uint64_t GetSeekPos(uint64_t fd);
		uint64_t OpenSocket(uint64_t domain, uint64_t type, uint64_t protocol);
		uint64_t BindNetSocket(uint64_t fd, uint32_t ipv4addr, uint16_t port);
		uint64_t ConnectNetSocket(uint64_t fd, uint32_t ipv4addr, uint16_t port);

		uint64_t BindIPCSocket(uint64_t fd, const char* path);
		uint64_t ConnectIPCSocket(uint64_t fd, const char* path);
	}
}
#else
#error unsupported
#endif











