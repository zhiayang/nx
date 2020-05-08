// load_store.s
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.


.global _ZN2nx6atomic4loadEPKa; .type _ZN2nx6atomic4loadEPKa, @function
.global _ZN2nx6atomic4loadEPKh; .type _ZN2nx6atomic4loadEPKh, @function
_ZN2nx6atomic4loadEPKa:
_ZN2nx6atomic4loadEPKh:
	// rdi: pointer to (u)int8_t
	lfence
	movb (%rdi), %al
	movzx %al, %rax
	ret


.global _ZN2nx6atomic4loadEPKs; .type _ZN2nx6atomic4loadEPKs, @function
.global _ZN2nx6atomic4loadEPKt; .type _ZN2nx6atomic4loadEPKt, @function
_ZN2nx6atomic4loadEPKs:
_ZN2nx6atomic4loadEPKt:
	// rdi: pointer to (u)int16_t
	lfence
	movw (%rdi), %ax
	movzx %ax, %rax
	ret


.global _ZN2nx6atomic4loadEPKi; .type _ZN2nx6atomic4loadEPKi, @function
.global _ZN2nx6atomic4loadEPKj; .type _ZN2nx6atomic4loadEPKj, @function
_ZN2nx6atomic4loadEPKi:
_ZN2nx6atomic4loadEPKj:
	// rdi: pointer to (u)int32_t
	lfence
	movl (%rdi), %eax
	ret


.global _ZN2nx6atomic4loadEPKl; .type _ZN2nx6atomic4loadEPKl, @function
.global _ZN2nx6atomic4loadEPKm; .type _ZN2nx6atomic4loadEPKm, @function
_ZN2nx6atomic4loadEPKl:
_ZN2nx6atomic4loadEPKm:
	// rdi: pointer to (u)int64_t
	lfence
	movq (%rdi), %rax
	ret









.global _ZN2nx6atomic5storeEPaa; .type _ZN2nx6atomic5storeEPaa, @function
.global _ZN2nx6atomic5storeEPhh; .type _ZN2nx6atomic5storeEPhh, @function
_ZN2nx6atomic5storeEPaa:
_ZN2nx6atomic5storeEPhh:
	// rdi: pointer to (u)int8_t
	// sil: value
	lock xchgb %sil, (%rdi)
	movzx %sil, %eax
	ret


.global _ZN2nx6atomic5storeEPss; .type _ZN2nx6atomic5storeEPss, @function
.global _ZN2nx6atomic5storeEPtt; .type _ZN2nx6atomic5storeEPtt, @function
_ZN2nx6atomic5storeEPss:
_ZN2nx6atomic5storeEPtt:
	// rdi: pointer to (u)int16_t
	// si: value
	lock xchgw %si, (%rdi)
	movzx %si, %eax
	ret


.global _ZN2nx6atomic5storeEPii; .type _ZN2nx6atomic5storeEPii, @function
.global _ZN2nx6atomic5storeEPjj; .type _ZN2nx6atomic5storeEPjj, @function
_ZN2nx6atomic5storeEPii:
_ZN2nx6atomic5storeEPjj:
	// rdi: pointer to (u)int32_t
	// esi: value
	lock xchgl %esi, (%rdi)
	mov %esi, %eax
	ret


.global _ZN2nx6atomic5storeEPll; .type _ZN2nx6atomic5storeEPll, @function
.global _ZN2nx6atomic5storeEPmm; .type _ZN2nx6atomic5storeEPmm, @function
_ZN2nx6atomic5storeEPll:
_ZN2nx6atomic5storeEPmm:
	// rdi: pointer to (u)int64_t
	// rsi: value
	lock xchgq %rsi, (%rdi)
	mov %rsi, %rax
	ret


.global _ZN2nx6atomic5storeEPvS1_; .type _ZN2nx6atomic5storeEPvS1_, @function
_ZN2nx6atomic5storeEPvS1_:
	// rdi: void**
	// rsi: void*
	lock xchgq %rsi, (%rdi)
	mov %rsi, %rax
	ret
