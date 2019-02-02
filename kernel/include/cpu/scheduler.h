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

			Process* currentProcess;
		};

		struct Process
		{
			addr_t cr3;
			nx::array<Thread> threads;

			extmm::State vmmStates[vmm::NumAddressSpaces];
		};

		static constexpr uint64_t FLAG_PREEMPTED    = 0x1;
		static constexpr uint64_t FLAG_MUTEX_BLOCK  = 0x2;

		struct Thread
		{
			addr_t userStack;
			addr_t kernelStack;

			Process* parent = 0;

			uint64_t flags = 0;
			mutex* blockedMtx = 0;
		};

		using Fn0Args_t = int64_t (*)();
		using Fn1Arg_t  = int64_t (*)(void*);
		using Fn2Args_t = int64_t (*)(void*, void*);
		using Fn3Args_t = int64_t (*)(void*, void*, void*);
		using Fn4Args_t = int64_t (*)(void*, void*, void*, void*);
		using Fn5Args_t = int64_t (*)(void*, void*, void*, void*, void*);
		using Fn6Args_t = int64_t (*)(void*, void*, void*, void*, void*, void*);

		// support up to 6 arguments
		Thread* createThread(Process* p, Fn0Args_t fn);
		Thread* createThread(Process* p, Fn1Arg_t  fn, void* a);
		Thread* createThread(Process* p, Fn2Args_t fn, void* a, void* b);
		Thread* createThread(Process* p, Fn3Args_t fn, void* a, void* b, void* c);
		Thread* createThread(Process* p, Fn4Args_t fn, void* a, void* b, void* c, void* d);
		Thread* createThread(Process* p, Fn5Args_t fn, void* a, void* b, void* c, void* d, void* e);
		Thread* createThread(Process* p, Fn6Args_t fn, void* a, void* b, void* c, void* d, void* e, void* f);

		void setupKernelProcess(addr_t cr3);

		Process* getKernelProcess();
		Process* getCurrentProcess();

		Thread* getNextThread();
		Thread* getCurrentThread();

		void init(Thread* idle_thread, Thread* work_thread);
		void setTickIRQ(int irq);

		void yield();
		void block(mutex* mtx);
		void unblock(mutex* mtx);

		void add(Thread* t);
		void add(Process* p);


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

















