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


		// note: don't mark stacks as NX, because GDB will die when we try to evaluate expressions
		// turns out, it just pushes opcodes to the stack and excecutes them. can't do that if NX.
		addr_t target_flags = vmm::PAGE_PRESENT | vmm::PAGE_WRITE; // | vmm::PAGE_NX;
		addr_t user_flags   = (isUserProc ? (vmm::PAGE_USER) : 0);


		/*
			now, the reason why differentiate between user and kernel threads is because we rely on
			the CPL3->CPL0 switch to move us to the RSP0 stack in the TSS. if we lazily-allocate the
			"user stack" for kernel threads, then a few problems happen:

			1.  to begin with, we never switch to the kernel stack after the first usage. we can't
				exactly "fix" this, which is kinda lame.

				since RSP0 is never used (we're always going CPL0 -> CPL0), after we IRET with the
				user-rsp to the "user stack", all ticks/interrupts/whatever will leave us on that
				same stack.


			2.  if we lazily allocate this stack, then the very first instruction that the thread
				runs will probably #PF (since function prologues usually push %rbp). this leads us
				to the #PF handler, which we can make run using an IST (since that is used unconditionally)

				for some reason, this causes us to be unable to send the EOI to the timer tick? idk
				wtf the problem is, and i'm too lazy to figure it out.


			so the solution here is to just... not have a user stack for kernel threads. we'll put
			the "user-rsp" value in the IRET stack to be the bottom of the IRET stack itself, and
			just run with that.
		*/

		auto u_stackTop = VirtAddr::zero();
		auto k_stackTop = VirtAddr::zero();
		if(isUserProc)
		{
			auto size = USER_STACK_SIZE / PAGE_SIZE;
			auto addr = vmm::allocate(size, vmm::AddressSpaceType::User, target_flags | user_flags, proc);

			u_stackTop = addr + USER_STACK_SIZE;
		}


		// finally, the kernel stack:
		{
			auto n = KERNEL_STACK_SIZE / PAGE_SIZE;

			auto phys = pmm::allocate(n);
			auto virt = vmm::allocateAddrSpace(n, vmm::AddressSpaceType::User, proc);
			LockedSection(&proc->addrSpaceLock, [&]() {
				proc->addrspace.addRegion(virt, phys, n);
			});

			// map it in the target address space. don't map it user-accessible.
			vmm::mapAddress(virt, phys, n, target_flags, proc);

			// ok, now allocate some space here -- as scratch so we can modify the pages.
			auto scratch = vmm::allocateAddrSpace(n, vmm::AddressSpaceType::User);

			vmm::mapAddress(scratch, phys, n, vmm::PAGE_PRESENT | vmm::PAGE_WRITE);

			constexpr size_t EXPECTED_STACK_OFFSET = 160;

			auto kstk = (uint64_t*) (scratch + KERNEL_STACK_SIZE).ptr();

			k_stackTop = virt + KERNEL_STACK_SIZE;

			auto userRSP = isUserProc
				? (u_stackTop)
				: (virt + KERNEL_STACK_SIZE - EXPECTED_STACK_OFFSET);

			auto codeSeg = isUserProc ? RING3_CODE_SEGMENT : RING0_CODE_SEGMENT;
			auto dataSeg = isUserProc ? RING3_STACK_SEGMENT : RING0_STACK_SEGMENT;

			*--kstk     = dataSeg;          // stack segment
			*--kstk     = userRSP.addr();   // stack pointer
			*--kstk     = 0x202;            // flags
			*--kstk     = codeSeg;          // code segment
			*--kstk     = (addr_t) fn;      // return addr


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


			assert((addr_t) kstk + EXPECTED_STACK_OFFSET == (scratch + KERNEL_STACK_SIZE).addr());

			// ok. set the stack.
			thr->kernelStack = (virt + KERNEL_STACK_SIZE - EXPECTED_STACK_OFFSET).addr();
			thr->kernelStackTop = (virt + KERNEL_STACK_SIZE).addr();

			// clear the temp mapping
			vmm::unmapAddress(scratch, n);
			vmm::deallocateAddrSpace(scratch, n);
		}


		// setup fpu state:
		{
			auto sseSz = cpu::fpu::getFPUStateSize();
			assert(sseSz <= PAGE_SIZE);

			auto phys = pmm::allocate(1);
			auto virt = vmm::allocateAddrSpace(1, vmm::AddressSpaceType::User, proc);

			// map scratch space.
			auto scratch = vmm::allocateAddrSpace(1, vmm::AddressSpaceType::User);
			vmm::mapAddress(scratch, phys, 1, vmm::PAGE_WRITE);

			cpu::fpu::initState(scratch.addr());

			vmm::unmapAddress(scratch, 1);

			vmm::mapAddress(virt, phys, 1, vmm::PAGE_PRESENT | vmm::PAGE_WRITE | vmm::PAGE_USER, proc);
			thr->fpuSavedStateBuffer = virt.addr();

			LockedSection(&proc->addrSpaceLock, [&]() {
				proc->addrspace.addRegion(virt, phys, 1);
			});
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
			assert(phys.nonZero());

			// note: we must map this user-accessible!
			auto virt = vmm::allocateAddrSpace(numPages, vmm::AddressSpaceType::User, proc);
			vmm::mapAddress(virt, phys, numPages, target_flags | user_flags, proc);
			LockedSection(&proc->addrSpaceLock, [&]() {
				proc->addrspace.addRegion(virt, phys, numPages);
			});


			auto scratch = vmm::allocateAddrSpace(numPages, vmm::AddressSpaceType::User);
			vmm::mapAddress(scratch, phys, numPages, vmm::PAGE_PRESENT | vmm::PAGE_WRITE);

			// setup the tcb first
			auto tcb = (usertcb_t*) (scratch + tcb_offset).ptr();
			memset(tcb, 0, sizeof(usertcb_t));

			tcb->self = (usertcb_t*) (virt + tcb_offset).ptr();
			tcb->threadId = thr->threadId;

			auto pcb = (userpcb_t*) (scratch + tcb_offset + sizeof(usertcb_t)).ptr();
			memset(pcb, 0, sizeof(userpcb_t));

			pcb->processId = proc->processId;

			tcb->parentProcess = (userpcb_t*) (virt + tcb_offset + sizeof(usertcb_t)).ptr();

			thr->userspaceTCB = tcb->self;
			thr->fsBase = (addr_t) tcb->self;



			if(proc->tlsSize > 0)
			{
				auto scratchMaster = vmm::allocateAddrSpace(numPages, vmm::AddressSpaceType::User);
				vmm::mapAddress(scratchMaster, vmm::getPhysAddr(VirtAddr(PAGE_ALIGN(proc->tlsMasterCopy)), proc),
					numPages, vmm::PAGE_PRESENT | vmm::PAGE_WRITE);

				scratchMaster += (proc->tlsMasterCopy - PAGE_ALIGN(proc->tlsMasterCopy));

				// ok now do the tls
				memmove((scratch + tcb_offset - tls_data_sz).ptr(), scratchMaster.ptr(), tls_data_sz);

				vmm::unmapAddress(scratchMaster.pageAligned(), numPages);
			}

			// ok we should be done.
			vmm::unmapAddress(scratch, numPages);
		}

		if(isUserProc)
		{
			log("sched", "created tid %lu in pid %lu [stks u: hi(%p) -> lo(%p), k: hi(%p) -> lo(%p)] [fpu: %p + %#x] [tls: %p]",
				thr->threadId, thr->parent->processId, u_stackTop, u_stackTop - USER_STACK_SIZE,
				k_stackTop, k_stackTop - KERNEL_STACK_SIZE, thr->fpuSavedStateBuffer, cpu::fpu::getFPUStateSize(),
				thr->userspaceTCB);
		}
		else
		{
			log("sched", "created kernel tid %lu [stk: hi(%p) -> lo(%p)] [fpu: %p + %#x] [tls: %p]",
				thr->threadId, k_stackTop, k_stackTop - KERNEL_STACK_SIZE,
				thr->fpuSavedStateBuffer, cpu::fpu::getFPUStateSize(),
				thr->userspaceTCB);
		}

		return thr;
	}




	void destroyThread(Thread* thr)
	{
		auto proc = thr->parent;
		assert(proc);

		// save this for later.
		auto id = thr->threadId;

		proc->threads.remove_all_if([&thr](const auto& t) -> bool {
			return t.threadId == thr->threadId;
		});

		log("sched", "destroyed thread %lu", id);

		if(proc->threads.empty())
		{
			log("sched", "killed last thread of process %lu, cleaning up process", proc->processId);
			destroyProcess(proc);
		}
	}
}
}



















