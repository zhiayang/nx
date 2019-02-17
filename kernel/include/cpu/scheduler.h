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

		struct CPU;

		// this is the per-cpu scheduler state!
		struct State
		{
			Thread* IdleThread = 0;
			Thread* CurrentThread = 0;

			nx::list<Thread*> ThreadList;
			nx::list<Thread*> BlockedThreads;
			nx::list<Thread*> DestructionQueue;

			nx::list<Process*> ProcessList;

			uint64_t tickCounter = 0;
		};

		// this might need to change on non-x64
		struct CPULocalState
		{
			// note: the first pointer should be a pointer to self, because apparently there's
			// no straightforward way to access %gs:0 without loading the value at %gs:0.
			// (the LEA instruction doesn't care about segments)

			CPULocalState* self = 0;        // ofs: 0x00

			int id = 0;                     // ofs: 0x08
			int lApicId = 0;                // ofs: 0x0C

			addr_t TSSBase = 0;             // ofs: 0x10
			uint64_t tssSelector = 0;       // ofs: 0x18

			addr_t cr3 = 0;                 // ofs: 0x20
			addr_t unused = 0;              // ofs: 0x28
			CPU* cpu = 0;                   // ofs: 0x30

			// do not change the position of stuff before this!!!
			// see syscall/syscall.s
			uint64_t syscallRetInstrPtr;    // ofs: 0x38
			uint64_t syscallRetStackPtr;    // ofs: 0x40
			uint64_t syscallRetRFlags;      // ofs: 0x48
		};

		struct CPU
		{
			int id;
			int lApicId;

			bool isBootstrap;
			addr_t localApicAddr;

			Process* currentProcess = 0;

			// the value of this pointer must match the KernelGSBase MSR!!
			CPULocalState* localState = 0;

			State schedState;
		};

		struct Process
		{
			pid_t processId;
			nx::string processName;

			addr_t cr3;
			nx::array<Thread> threads;

			int flags;

			addr_t tlsMasterCopy;
			size_t tlsAlign = 0;
			size_t tlsSize = 0;

			nx::mutex msgQueueLock;
			nx::list<ipc::message_t*> pendingMessages;

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

			addr_t userStackBottom;

			addr_t kernelStackTop;
			addr_t kernelStackBottom;

			Process* parent = 0;

			ThreadState state = ThreadState::Stopped;

			mutex* blockedMtx = 0;
			uint64_t wakeUpTimestamp = 0;

			// we don't really care about the data that goes here.
			void* userspaceTCB;

			// saved information from context switches:
			addr_t kernelStack;
			addr_t fsBase;
			addr_t fpuSavedStateBuffer;
		};


		void setupKernelProcess(addr_t cr3);

		Process* getKernelProcess();
		Process* getCurrentProcess();

		Process* createProcess(const nx::string& name, int flags);
		void addProcess(Process* p);

		Process* getProcessWithId(pid_t id);











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

		State* getSchedState();

		// call these 4 functions in this order to start scheduling.
		void bootstrap();
		void init();
		void installTickHandlers();
		void start();

		void setTickIRQ(int irq);

		uint64_t getElapsedNanoseconds();
		uint64_t getNanosecondsPerTick();


		[[noreturn]] void exit(int status);

		void yield();
		void sleep(uint64_t ns);

		void block(mutex* mtx);
		void unblock(mutex* mtx);

		void addThread(Thread* t);

		void destroyThread(Thread* t);
		void destroyProcess(Process* p);


		enum class SchedulerInitPhase
		{
			Uninitialised,
			KernelProcessInit,
			ReadyToRegisterCPUs,
			BootstrapCPURegistered,
			SchedulerStarted
		};

		// this is kinda dumb
		SchedulerInitPhase getInitPhase();
		void setInitPhase(SchedulerInitPhase phase);

		// processor-related stuff

		void preinitCPUs();
		void registerCPU(bool bsp, int id, int lApicId, addr_t localApic);

		CPULocalState* getCPULocalState();

		size_t getNumCPUs();

		CPU* getCurrentCPU();
		nx::array<CPU>& getAllCPUs();

		void initCPU(CPU* cpu);
	};
}

















