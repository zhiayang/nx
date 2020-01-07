// thread.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

#include "export/thread.h"

namespace nx {
namespace scheduler
{
	constexpr size_t KernelStackSize = 0x4000;

	Thread* createThread(Process* p, Fn0Args_t fn)
	{
		return createThread(p, (Fn6Args_t) (void*) fn, 0, 0, 0, 0, 0, 0);
	}
	Thread* createThread(Process* p, Fn1Arg_t fn, void* a)
	{
		return createThread(p, (Fn6Args_t) (void*) fn, a, 0, 0, 0, 0, 0);
	}
	Thread* createThread(Process* p, Fn2Args_t fn, void* a, void* b)
	{
		return createThread(p, (Fn6Args_t) (void*) fn, a, b, 0, 0, 0, 0);
	}
	Thread* createThread(Process* p, Fn3Args_t fn, void* a, void* b, void* c)
	{
		return createThread(p, (Fn6Args_t) (void*) fn, a, b, c, 0, 0, 0);
	}
	Thread* createThread(Process* p, Fn4Args_t fn, void* a, void* b, void* c, void* d)
	{
		return createThread(p, (Fn6Args_t) (void*) fn, a, b, c, d, 0, 0);
	}
	Thread* createThread(Process* p, Fn5Args_t fn, void* a, void* b, void* c, void* d, void* e)
	{
		return createThread(p, (Fn6Args_t) (void*) fn, a, b, c, d, e, 0);
	}



	constexpr uint64_t RING0_CODE_SEGMENT        = 0x08;
	constexpr uint64_t RING0_STACK_SEGMENT       = 0x10;     // 0x18 | 0x3

	// see cpu/gdt.cpp and syscall/funcs.cpp for why the code segment is *after* the data segment
	constexpr uint64_t RING3_STACK_SEGMENT       = 0x23;     // 0x20 | 0x3
	constexpr uint64_t RING3_CODE_SEGMENT        = 0x2B;     // 0x28 | 0x3


	//* note: we don't bother rounding this up, we just divide by PAGE_SIZE
	//* so don't be stupid; set these to a multiple of 0x1000, thanks.
	constexpr size_t KERNEL_STACK_SIZE           = 0x2000;
	constexpr size_t USER_STACK_SIZE             = 0x4000;

	static pid_t ThreadIdCounter = 0;
	Thread* createThread(Process* proc, Fn6Args_t fn, void* a, void* b, void* c, void* d, void* e, void* f)
	{
		bool isUserProc = (proc->flags & Process::PROC_USER);

		// setup the kernel stack

		assert(proc);
		proc->threads.append(Thread());

		auto thr = &proc->threads.back();
		thr->parent = proc;

		thr->threadId = ThreadIdCounter++;

		// allocate the user and kernel stacks, but in the address space of the process.
		// we don't really care what the address is, as long as we got the space.
		// we do the phys and virt allocation separately because we want the physical address as well.

		addr_t target_flags = vmm::PAGE_PRESENT | vmm::PAGE_WRITE | vmm::PAGE_NX | (isUserProc ? (vmm::PAGE_USER) : 0);

		// first, just make some space for the user stack.
		{
			auto n = USER_STACK_SIZE / PAGE_SIZE;

			auto phys = pmm::allocate(n);
			auto virt = vmm::allocateAddrSpace(n, vmm::AddressSpace::User, proc);

			// map it in the target address space
			vmm::mapAddress(virt, phys, n, target_flags, proc);

			// we don't need to map it in our address space, because we don't need to write anything there.
			// (yet!) -- eventually we want to write a return address that will kill the thread.
			thr->userStackBottom = virt;
		}


		// then, the kernel stack:
		{
			auto n = KERNEL_STACK_SIZE / PAGE_SIZE;

			auto phys = pmm::allocate(n);
			auto virt = vmm::allocateAddrSpace(n, vmm::AddressSpace::User, proc);

			// save it so we can kill it later.
			thr->kernelStackBottom = virt;

			// map it in the target address space
			vmm::mapAddress(virt, phys, n, target_flags, proc);

			// ok, now allocate some space here -- as scratch so we can modify the pages.
			auto scratch = vmm::allocateAddrSpace(n, vmm::AddressSpace::User);

			vmm::mapAddress(scratch, phys, n, vmm::PAGE_PRESENT | vmm::PAGE_WRITE);

			auto kstk = (uint64_t*) (scratch + KERNEL_STACK_SIZE);

			*--kstk     = (isUserProc ? RING3_STACK_SEGMENT : RING0_STACK_SEGMENT); // stack segment
			*--kstk     = thr->userStackBottom + USER_STACK_SIZE;                   // stack pointer
			*--kstk     = 0x202;                                                    // flags
			*--kstk     = (isUserProc ? RING3_CODE_SEGMENT : RING0_CODE_SEGMENT);   // code segment
			*--kstk     = (addr_t) fn;                                              // return addr


			// now for the registers.
			*--kstk     = 0;            // r15
			*--kstk     = 0;            // r14
			*--kstk     = 0;            // r13
			*--kstk     = 0;            // r12
			*--kstk     = 0;            // r11
			*--kstk     = 0;            // r10
			*--kstk     = (uint64_t) f; // r9
			*--kstk     = (uint64_t) e; // r8
			*--kstk     = (uint64_t) c; // rdx
			*--kstk     = (uint64_t) d; // rcx
			*--kstk     = 0;            // rbx
			*--kstk     = 0;            // rax
			*--kstk     = 0;            // rbp
			*--kstk     = (uint64_t) b; // rsi
			*--kstk     = (uint64_t) a; // rdi


			constexpr size_t EXPECTED_STACK_OFFSET = 160;
			assert((addr_t) kstk + EXPECTED_STACK_OFFSET == (scratch + KERNEL_STACK_SIZE));

			// ok. set the stack.
			thr->kernelStack = (virt + KERNEL_STACK_SIZE) - EXPECTED_STACK_OFFSET;
			thr->kernelStackTop = (virt + KERNEL_STACK_SIZE);

			// clear the temp mapping
			vmm::unmapAddress(scratch, n, /* freePhys: */ false);
			vmm::deallocateAddrSpace(scratch, n);
		}


		// setup fpu state:
		{
			auto sseSz = cpu::fpu::getFPUStateSize();
			assert(sseSz <= PAGE_SIZE);

			auto phys = pmm::allocate(1);
			auto virt = vmm::allocateAddrSpace(1, vmm::AddressSpace::User, proc);

			// map scratch space.
			auto scratch = vmm::allocateAddrSpace(1, vmm::AddressSpace::User);
			vmm::mapAddress(scratch, phys, 1, vmm::PAGE_WRITE);

			cpu::fpu::initState(scratch);

			vmm::unmapAddress(scratch, 1, /* freePhys: */ false);

			vmm::mapAddress(virt, phys, 1, vmm::PAGE_PRESENT | vmm::PAGE_WRITE | vmm::PAGE_USER, proc);
			thr->fpuSavedStateBuffer = virt;
		}


		// thread-local storage:
		{
			// TODO: find some way to minimise wastage here?
			//* we are always allocating a minimum of 4kb for each thread, even if it only
			//* has one thread local variable eg. errno.

			// note: we always need to allocate tls -- for the thread control block.


			// so we can distinguish it.
			using usertcb_t = thread_t;
			using userpcb_t = process_t;

			size_t tls_data_sz = proc->tlsSize ? __alignup(proc->tlsSize, proc->tlsAlign) : 0;
			size_t tcb_offset  = __alignup(proc->tlsSize, __max(proc->tlsAlign,
				__max(alignof(usertcb_t), alignof(userpcb_t))));


			// ok, we need to allocate pages for this as well.
			auto numPages = SIZE_IN_PAGES(tcb_offset + sizeof(usertcb_t) + sizeof(userpcb_t));

			auto phys = pmm::allocate(numPages);
			assert(phys);

			auto virt = vmm::allocateAddrSpace(numPages, vmm::AddressSpace::User, proc);
			vmm::mapAddress(virt, phys, numPages, target_flags, proc);

			auto scratch = vmm::allocateAddrSpace(numPages, vmm::AddressSpace::User);
			vmm::mapAddress(scratch, phys, numPages, vmm::PAGE_PRESENT | vmm::PAGE_WRITE);

			// setup the tcb first
			auto tcb = (usertcb_t*) (scratch + tcb_offset);
			memset(tcb, 0, sizeof(usertcb_t));

			tcb->self = (usertcb_t*) (virt + tcb_offset);
			tcb->threadId = thr->threadId;

			auto pcb = (userpcb_t*) (scratch + tcb_offset + sizeof(usertcb_t));
			memset(pcb, 0, sizeof(userpcb_t));

			pcb->processId = proc->processId;

			tcb->parentProcess = (userpcb_t*) (virt + tcb_offset + sizeof(usertcb_t));

			thr->userspaceTCB = tcb->self;
			thr->fsBase = (addr_t) tcb->self;



			if(proc->tlsSize > 0)
			{
				auto scratchMaster = vmm::allocateAddrSpace(numPages, vmm::AddressSpace::User);
				vmm::mapAddress(scratchMaster, vmm::getPhysAddr(proc->tlsMasterCopy & vmm::PAGE_ALIGN, proc), numPages,
					vmm::PAGE_PRESENT | vmm::PAGE_WRITE);

				scratchMaster += (proc->tlsMasterCopy - (proc->tlsMasterCopy & vmm::PAGE_ALIGN));

				// ok now do the tls
				memmove((void*) (scratch + tcb_offset - tls_data_sz), (void*) scratchMaster, tls_data_sz);

				vmm::unmapAddress(scratchMaster & vmm::PAGE_ALIGN, numPages, /* freePhys: */ false);
			}


			// ok we should be done.
			vmm::unmapAddress(scratch, numPages, /* freePhys: */ false);

		}

		return thr;
	}




	void destroyThread(Thread* thr)
	{
		auto proc = thr->parent;
		assert(proc);

		// kill the user stack.
		vmm::deallocate(thr->userStackBottom, USER_STACK_SIZE / PAGE_SIZE, proc);

		// kill the kernel stack.
		vmm::deallocate(thr->kernelStackBottom, KERNEL_STACK_SIZE / PAGE_SIZE, proc);

		// kill the sse state
		vmm::deallocate(thr->fpuSavedStateBuffer, 1, proc);

		// save this for later.
		auto id = thr->threadId;


		// it needs to disappear from the scheduler state too
		proc->threads.remove_all_if([&thr](const auto& t) -> bool {
			return t.threadId == thr->threadId;
		});

		getSchedState()->ThreadList.remove_all(thr);


		log("sched", "destroyed thread %lu", id);

		if(proc->threads.empty())
		{
			log("sched", "killed last thread of process %lu, cleaning up process", proc->processId);
			destroyProcess(proc);
		}
	}
}
}



















