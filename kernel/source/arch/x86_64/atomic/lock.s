// lock.s
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.






.global _ZN2nx6atomic12cas_spinlockEPmmm;
.type _ZN2nx6atomic12cas_spinlockEPmmm, @function
_ZN2nx6atomic12cas_spinlockEPmmm:
	// rdi: pointer to (u)int64_t
	// rsi: expected value
	// rdx: new value

	// load rax first
	mov %rsi, %rax

	// optimistically try to get the lock first. if we succeed, then return.
	// this will check if (%rdi) == rax; if so, then set (rdi) = %rdx
	lock cmpxchgq %rdx, (%rdi)
	jz 2f

	// we didn't succeed; use 'pause' to ask the cpu to chill while we try again.
1:
	pause

	// see if we have the original value; if not, then don't bother trying to lock
	cmp %rsi, (%rdi)
	jne 1b

	lock cmpxchgq %rdx, (%rdi)
	jnz 1b

2:
	// success
	ret



.global _ZN2nx6atomic11cas_trylockEPmmm;
.type _ZN2nx6atomic11cas_trylockEPmmm, @function
_ZN2nx6atomic11cas_trylockEPmmm:
	// rdi: pointer to (u)int64_t
	// rsi: expected value
	// rdx: new value

	// load rax first
	mov %rsi, %rax

	lock cmpxchgq %rdx, (%rdi)

	// zero flag set on success
	setz %al
	movzx %al, %rax
	ret







// also disables irqs when locked.
.global _ZN2nx6atomic18cas_spinlock_noirqEPmmm;
.type _ZN2nx6atomic18cas_spinlock_noirqEPmmm, @function
_ZN2nx6atomic18cas_spinlock_noirqEPmmm:
	// rdi: pointer to (u)int64_t
	// rsi: expected value
	// rdx: new value

	// load rax first
	mov %rsi, %rax

	// optimistically try to get the lock first. if we succeed, then return.
	// push rflags, and disable interrupts in our optimism.
	pushfq
	cli

	// this will check if (%rdi) == rax; if so, then set (rdi) = %rdx
	lock cmpxchgq %rdx, (%rdi)
	jz 2f

	// we didn't succeed; use 'pause' to ask the cpu to chill while we try again.
1:
	popfq       // restore the old flags
	pushfq      // and push them again.

	pause

	// see if we have the original value; if not, then don't bother trying to lock
	cmp %rsi, (%rdi)
	jne 1b

	// optimistically disable interrupts, again
	cli

	lock cmpxchgq %rdx, (%rdi)
	jnz 1b

2:
	// success. interrupts are already disabled, so restore the stack (remove the flags)
	addq $8, %rsp

	// and update the kernel interrupt-sti-level counter.
	call _ZN2nx10interrupts7disableEv
	ret




.global _ZN2nx6atomic17cas_trylock_noirqEPmmm;
.type _ZN2nx6atomic17cas_trylock_noirqEPmmm, @function
_ZN2nx6atomic17cas_trylock_noirqEPmmm:
	// rdi: pointer to (u)int64_t
	// rsi: expected value
	// rdx: new value

	// load rax first
	mov %rsi, %rax

	// push rflags, and disable interrupts in our optimism.
	pushfq
	cli

	// try the lock.
	lock cmpxchgq %rdx, (%rdi)
	jz 2f

	// we failed; restore the flags (potentially re-enabling interrupts).
	popfq
	xor %eax, %eax
	ret

2:
	// success; update the kernel sti-level counter, and remove the flags from the stack.
	addq $8, %rsp
	call _ZN2nx10interrupts7disableEv

	movl $1, %eax
	ret
