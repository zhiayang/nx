// scheduler.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"

#include "defs.h"

#include "mm.h"
#include "misc/addrs.h"

namespace nx
{
	namespace scheduler
	{
		struct Thread;
		struct Process;

		struct CPU
		{
			int id;
			int lApicId;

			bool isBootstrap;
			addr_t localApicAddr;

			Process* currentProcess = 0;
		};

		struct Process
		{
			pid_t processId;
			nx::string processName;

			addr_t cr3;
			nx::array<Thread> threads;

			int flags;

			extmm::State vmmStates[vmm::NumAddressSpaces];


			static constexpr int PROC_USER      = 0x1;
			static constexpr int PROC_DRIVER    = 0x2;
		};

		enum class ThreadState
		{
			Stopped         = 0,
			Running         = 1,
			BlockedOnMutex  = 2,
			BlockedOnSleep  = 3,
			AboutToExit     = 4,
		};

		struct Thread
		{
			pid_t threadId;

			addr_t userStack;

			addr_t kernelStack;
			addr_t kernelStackTop;

			Process* parent = 0;

			ThreadState state = ThreadState::Stopped;

			mutex* blockedMtx = 0;
			uint64_t wakeUpTimestamp = 0;
		};


		void setupKernelProcess(addr_t cr3);

		Process* getKernelProcess();
		Process* getCurrentProcess();

		Process* createProcess(const nx::string& name, int flags);
		void addProcess(Process* p);













		using Fn0Args_t = void (*)();
		using Fn1Arg_t  = void (*)(void*);
		using Fn2Args_t = void (*)(void*, void*);
		using Fn3Args_t = void (*)(void*, void*, void*);
		using Fn4Args_t = void (*)(void*, void*, void*, void*);
		using Fn5Args_t = void (*)(void*, void*, void*, void*, void*);
		using Fn6Args_t = void (*)(void*, void*, void*, void*, void*, void*);

		// support up to 6 arguments
		Thread* createThread(Process* p, Fn0Args_t fn);
		Thread* createThread(Process* p, Fn1Arg_t  fn, void* a);
		Thread* createThread(Process* p, Fn2Args_t fn, void* a, void* b);
		Thread* createThread(Process* p, Fn3Args_t fn, void* a, void* b, void* c);
		Thread* createThread(Process* p, Fn4Args_t fn, void* a, void* b, void* c, void* d);
		Thread* createThread(Process* p, Fn5Args_t fn, void* a, void* b, void* c, void* d, void* e);
		Thread* createThread(Process* p, Fn6Args_t fn, void* a, void* b, void* c, void* d, void* e, void* f);

		Thread* getCurrentThread();

		void init(Thread* idle_thread, Thread* work_thread);
		void setTickIRQ(int irq);

		uint64_t getElapsedNanoseconds();
		uint64_t getNanosecondsPerTick();


		[[noreturn]] void exit();

		void yield();
		void sleep(uint64_t ns);

		void block(mutex* mtx);
		void unblock(mutex* mtx);

		void addThread(Thread* t);

		void destroyThread(Thread* t);


		// this is kinda dumb
		int getCurrentInitialisationPhase();
		void initialisePhase(int p);



		// processor-related stuff

		void preinitCPUs();
		void registerCPU(bool bsp, int id, int lApicId, addr_t localApic);

		size_t getNumCPUs();

		CPU* getCurrentCPU();
	};
}

















