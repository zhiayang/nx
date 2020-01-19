// scheduler.h
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stddef.h>
#include "stdint.h"

#include "defs.h"

#include "mm.h"
#include "ipc.h"
#include "cpu.h"
#include "extmm.h"
#include "synchro.h"
#include "misc/addrs.h"

namespace nx
{
	namespace mm
	{
		struct SharedMemoryBuffer
		{
			int64_t handle = 0;

			// we don't need an explicit refcount, the list of referring processes
			// will just act as the refcount anyway.
			nx::list<scheduler::Process*> procs;


			// i think this might come back to bite us in the ass when we do on-demand
			// paging?? eventually we definitely don't want to eagerly allocate every
			// page here... for now this will suffice ):
		};
	}

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
			bool expediteSchedule = false;
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
			addr_t tmpUserRsp = 0;          // ofs: 0x38
			Thread* currentThread = 0;      // ofs: 0x40
		};

		// these need to be used from ASM, so we must make sure they never change!
		static_assert(offsetof(CPULocalState, self)         == 0x00);
		static_assert(offsetof(CPULocalState, id)           == 0x08);
		static_assert(offsetof(CPULocalState, lApicId)      == 0x0C);
		static_assert(offsetof(CPULocalState, TSSBase)      == 0x10);
		static_assert(offsetof(CPULocalState, tssSelector)  == 0x18);
		static_assert(offsetof(CPULocalState, cr3)          == 0x20);
		// 0x28 is currently unused.
		static_assert(offsetof(CPULocalState, cpu)          == 0x30);
		static_assert(offsetof(CPULocalState, tmpUserRsp)   == 0x38);
		static_assert(offsetof(CPULocalState, currentThread)== 0x40);


		struct CPU
		{
			int id = 0;
			int lApicId = 0;

			bool isBootstrap = 0;
			addr_t localApicAddr = 0;

			Process* currentProcess = 0;

			// the value of this pointer must match the KernelGSBase MSR!!
			CPULocalState* localState = 0;

			State schedState;
		};

		struct Process
		{
			pid_t processId = 0;
			nx::string processName;

			nx::array<Thread> threads;

			int flags = 0;

			addr_t tlsMasterCopy = 0;
			size_t tlsAlign = 0;
			size_t tlsSize = 0;

			nx::mutex msgQueueLock;
			nx::list<ipc::message_t> pendingMessages;

			// see the paragraphs in _heap_impl.h on why this is a spinlock and not
			// a mutex -- the same concept applies, cos we might need to call mapAddress() to
			// expand the heap.
			nx::spinlock addrSpaceLock; // this locks both of these below.
			extmm::State<> vmmStates[vmm::NumAddressSpaces];
			vmm::AddressSpace addrspace;


			vfs::IOCtx* ioctx = 0;

			static constexpr int PROC_USER      = 0x1;
			static constexpr int PROC_DRIVER    = 0x2;

			// arch-specific stuff.
			#ifdef __ARCH_x64__

			// this is a map from the offset to the byte value at that offset, in the iopb.
			// if there is no value that offset, we presume it to be the default, ie 0xFF.
			nx::treemap<uint16_t, uint8_t> ioPorts;

			#endif
		};

		enum class ThreadState
		{
			Stopped         = 0,
			Running         = 1,
			BlockedOnMutex  = 2,
			BlockedOnSleep  = 3,
			AboutToExit     = 4,
		};

		struct Priority
		{
			// the base priority of the item.
			uint64_t basePriority = 0;

			// when was the last time (in terms of the system nanosecond timestamp)
			// this thing was given a chance to run.
			uint64_t lastScheduled = 0;

			// how much extra priority we should get, using the formula
			// (currentTime() - lastScheduled) / starvationRewardDivisor
			// note: this is a divisor cos the times are in nanoseconds, and everything will
			// quickly balloon if we just use the raw values.
			uint64_t starvationRewardDivisor = 1;

			// you can use this to boost the thread's priority for the next schedule event;
			// will be reset with reset().
			uint64_t temporaryBoost = 0;

			// how much to boost at a time when you call boost()
			uint64_t boostMultiplier = 0;

			// call this when you get scheduled.
			void reset();

			// boosts the thread's priority
			void boost(uint64_t amt = 1);

			// get the effective priority of the thread.
			uint64_t effective() const;


			bool operator > (const Priority& other) const { return this->effective() > other.effective(); }

			static Priority low();
			static Priority normal();
			static Priority high();
		};

		struct Thread
		{
			pid_t threadId = 0;


			Priority priority = Priority::normal();

			Process* parent = 0;

			uint64_t flags = 0;
			ThreadState state = ThreadState::Stopped;

			mutex* blockedMtx = 0;
			uint64_t wakeUpTimestamp = 0;


			nx::list<PageExtent> cleanupPages;


			// saved information from context switches:
			addr_t kernelStackTop = 0;
			addr_t kernelStack;

			addr_t fsBase;
			addr_t fpuSavedStateBuffer;

			// we don't really care about the data that goes here.
			void* userspaceTCB;



			// signal stuff.
			// these functions point to USERSPACE CODE!
			ipc::signal_handler_fn_t signalHandlers[ipc::MAX_SIGNAL_TYPES] = { 0 };

			bool pendingSignalRestore = false;


			// these things get touched in critical sections, so we use the special
			// allocator to make sure they don't try to acquire locks in the main
			// kernel heap, which would be a bad thing.
			krt::list<ipc::signal_message_t, _fixed_allocator, _aborter> pendingSignalQueue;
			krt::list<cpu::InterruptedState, _fixed_allocator, _aborter> savedSignalStateStack;
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

		void allowProcessIOPorts(Process* p, const nx::array<krt::pair<uint16_t, size_t>>& allowedPorts);
		void allowProcessIOPort(Process* p, uint16_t port);


		Thread* getCurrentThread();

		State* getSchedState();

		// call these 4 functions in this order to start scheduling.
		void bootstrap();
		void init();
		void installTickHandlers();
		void start();

		void setTickIRQ(int irq);
		int getTickIRQ();

		uint64_t getElapsedNanoseconds();
		uint64_t getNanosecondsPerTick();


		[[noreturn]] void exit(int status);

		void terminate(Thread* p);
		void terminate(Process* p);

		void yield();
		void sleep(uint64_t ns);

		void block(mutex* mtx);
		void unblock(mutex* mtx);

		// returns t.
		Thread* addThread(Thread* t);
		Thread* pauseThread(Thread* t);
		Thread* resumeThread(Thread* t);

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
	}
}

















