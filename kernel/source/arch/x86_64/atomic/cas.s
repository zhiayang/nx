// cas.s
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.



// bool nx::atomic::cas16(void*, void*, void*)

.global _ZN2nx6atomic5cas16EPvS1_S1_
.type _ZN2nx6atomic5cas16EPvS1_S1_, @function

_ZN2nx6atomic5cas16EPvS1_S1_:
	// rdi: the address of your 16-byte value
	// rsi: a pointer to the expected value
	// rdx: a pointer to the new value

	// we need to preserve rbx. so save it in r9 first.
	mov %rbx, %r9

	// since we need to *use* rdx as part of the cmpxchg, save it to r8
	mov %rdx, %r8

	// setup the expected value in rdx:rax
	movq 8(%rsi), %rdx
	movq 0(%rsi), %rax

	// setup the new value in rcx:rbx
	movq 8(%r8), %rcx
	movq 0(%r8), %rbx

	lock cmpxchg16b (%rdi)

	// restore rbx
	mov %r9, %rbx

	jz 1f

	// if the thing failed, we need to return the values back to the caller.
	// rsi still contains the second argument, so use that.
	movq %rdx, 8(%rsi)
	movq %rax, 0(%rsi)

	// clear it.
	xor %eax, %eax
	ret
1:
	movl $1, %eax
	ret






// bool nx::atomic::cas(int8_t*, int8_t*, int8_t)
.global _ZN2nx6atomic9cas_fetchEPaS1_a; .type _ZN2nx6atomic9cas_fetchEPaS1_a, @function
.global _ZN2nx6atomic9cas_fetchEPhS1_h; .type _ZN2nx6atomic9cas_fetchEPhS1_h, @function
_ZN2nx6atomic9cas_fetchEPaS1_a:
_ZN2nx6atomic9cas_fetchEPhS1_h:
	// rdi: pointer to (u)int8_t
	// rsi: pointer to expected value (also, output of old value)
	// rdx: new value

	// load rax first
	movb (%rsi), %al

	// this will check if (%rdi) == al; if so, then set (rdi) = %dl
	lock cmpxchgb %dl, (%rdi)
	jz 1f

	// store the real value back to the pointer if we failed.
	movb %al, (%rsi)
	xor %eax, %eax
	ret
1:
	// success
	movl $1, %eax
	ret


// bool nx::atomic::cas(int16_t*, int16_t*, int16_t)
.global _ZN2nx6atomic9cas_fetchEPsS1_s; .type _ZN2nx6atomic9cas_fetchEPsS1_s, @function
.global _ZN2nx6atomic9cas_fetchEPtS1_t; .type _ZN2nx6atomic9cas_fetchEPtS1_t, @function
_ZN2nx6atomic9cas_fetchEPsS1_s:
_ZN2nx6atomic9cas_fetchEPtS1_t:
	// rdi: pointer to (u)int16_t
	// rsi: pointer to expected value (also, output of old value)
	// rdx: new value

	// load rax first
	movw (%rsi), %ax

	// this will check if (%rdi) == ax; if so, then set (rdi) = %dx
	lock cmpxchgw %dx, (%rdi)
	jz 1f

	// store the real value back to the pointer if we failed.
	movw %ax, (%rsi)
	xor %eax, %eax
	ret
1:
	// success
	movl $1, %eax
	ret



// bool nx::atomic::cas(int32_t*, int32_t*, int32_t)
.global _ZN2nx6atomic9cas_fetchEPiS1_i; .type _ZN2nx6atomic9cas_fetchEPiS1_i, @function
.global _ZN2nx6atomic9cas_fetchEPjS1_j; .type _ZN2nx6atomic9cas_fetchEPjS1_j, @function
_ZN2nx6atomic9cas_fetchEPiS1_i:
_ZN2nx6atomic9cas_fetchEPjS1_j:
	// rdi: pointer to (u)int32_t
	// rsi: pointer to expected value (also, output of old value)
	// rdx: new value

	// load rax first
	movl (%rsi), %eax

	// this will check if (%rdi) == eax; if so, then set (rdi) = %edx
	lock cmpxchgl %edx, (%rdi)
	jz 1f

	// store the real value back to the pointer if we failed.
	movl %eax, (%rsi)
	xor %eax, %eax
	ret
1:
	// success
	movl $1, %eax
	ret



// bool nx::atomic::cas(int64_t*, int64_t*, int64_t)
.global _ZN2nx6atomic9cas_fetchEPlS1_l; .type _ZN2nx6atomic9cas_fetchEPlS1_l, @function
.global _ZN2nx6atomic9cas_fetchEPmS1_m; .type _ZN2nx6atomic9cas_fetchEPmS1_m, @function
_ZN2nx6atomic9cas_fetchEPlS1_l:
_ZN2nx6atomic9cas_fetchEPmS1_m:
	// rdi: pointer to (u)int64_t
	// rsi: pointer to expected value (also, output of old value)
	// rdx: new value

	// load rax first
	mov (%rsi), %rax

	// this will check if (%rdi) == rax; if so, then set (rdi) = %rdx
	lock cmpxchgq %rdx, (%rdi)
	jz 1f

	// store the real value back to the pointer if we failed.
	mov %rax, (%rsi)
	xor %eax, %eax
	ret
1:
	// success
	movl $1, %eax
	ret







// variants of CAS that don't care about retrieving the old value
// ie. the second argument is the value.


// bool nx::atomic::cas(int8_t*, int8_t, int8_t)
.global _ZN2nx6atomic3casEPaaa; .type _ZN2nx6atomic3casEPaaa, @function
.global _ZN2nx6atomic3casEPhhh; .type _ZN2nx6atomic3casEPhhh, @function
_ZN2nx6atomic3casEPaaa:
_ZN2nx6atomic3casEPhhh:
	// rdi: pointer to (u)int8_t
	// rsi: expected value
	// rdx: new value

	mov %sil, %al

	lock cmpxchgb %dl, (%rdi)
	setz %al
	movzx %al, %rax
	ret


// bool nx::atomic::cas(int16_t*, int16_t, int16_t)
.global _ZN2nx6atomic3casEPsss; .type _ZN2nx6atomic3casEPsss, @function
.global _ZN2nx6atomic3casEPttt; .type _ZN2nx6atomic3casEPttt, @function
_ZN2nx6atomic3casEPsss:
_ZN2nx6atomic3casEPttt:
	// rdi: pointer to (u)int16_t
	// rsi: expected value
	// rdx: new value

	mov %si, %ax

	lock cmpxchgw %dx, (%rdi)
	setz %al
	movzx %al, %rax
	ret



// bool nx::atomic::cas(int32_t*, int32_t, int32_t)
.global _ZN2nx6atomic3casEPiii; .type _ZN2nx6atomic3casEPiii, @function
.global _ZN2nx6atomic3casEPjjj; .type _ZN2nx6atomic3casEPjjj, @function
_ZN2nx6atomic3casEPiii:
_ZN2nx6atomic3casEPjjj:
	// rdi: pointer to (u)int32_t
	// rsi: expected value
	// rdx: new value

	mov %esi, %eax

	lock cmpxchgl %edx, (%rdi)
	setz %al
	movzx %al, %rax
	ret



// bool nx::atomic::cas(int64_t*, int64_t, int64_t)
.global _ZN2nx6atomic3casEPlll; .type _ZN2nx6atomic3casEPlll, @function
.global _ZN2nx6atomic3casEPmmm; .type _ZN2nx6atomic3casEPmmm, @function
_ZN2nx6atomic3casEPlll:
_ZN2nx6atomic3casEPmmm:
	// rdi: pointer to (u)int64_t
	// rsi: expected value
	// rdx: new value

	mov %rsi, %rax

	lock cmpxchgq %rdx, (%rdi)
	setz %al
	movzx %al, %rax
	ret
