// load_store.s
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.


.global _ZN2nx6atomic4loadEPa; .type _ZN2nx6atomic4loadEPa, @function
.global _ZN2nx6atomic4loadEPh; .type _ZN2nx6atomic4loadEPh, @function
_ZN2nx6atomic4loadEPa:
_ZN2nx6atomic4loadEPh:
	// rdi: pointer to (u)int8_t
	lfence
	movb (%rdi), %al
	movzx %al, %rax
	ret


.global _ZN2nx6atomic4loadEPs; .type _ZN2nx6atomic4loadEPs, @function
.global _ZN2nx6atomic4loadEPt; .type _ZN2nx6atomic4loadEPt, @function
_ZN2nx6atomic4loadEPs:
_ZN2nx6atomic4loadEPt:
	// rdi: pointer to (u)int16_t
	lfence
	movw (%rdi), %ax
	movzx %ax, %rax
	ret


.global _ZN2nx6atomic4loadEPi; .type _ZN2nx6atomic4loadEPi, @function
.global _ZN2nx6atomic4loadEPj; .type _ZN2nx6atomic4loadEPj, @function
_ZN2nx6atomic4loadEPi:
_ZN2nx6atomic4loadEPj:
	// rdi: pointer to (u)int32_t
	lfence
	movl (%rdi), %eax
	ret


.global _ZN2nx6atomic4loadEPl; .type _ZN2nx6atomic4loadEPl, @function
.global _ZN2nx6atomic4loadEPm; .type _ZN2nx6atomic4loadEPm, @function
_ZN2nx6atomic4loadEPl:
_ZN2nx6atomic4loadEPm:
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
	sfence
	movb %sil, (%rdi)
	ret


.global _ZN2nx6atomic5storeEPss; .type _ZN2nx6atomic5storeEPss, @function
.global _ZN2nx6atomic5storeEPtt; .type _ZN2nx6atomic5storeEPtt, @function
_ZN2nx6atomic5storeEPss:
_ZN2nx6atomic5storeEPtt:
	// rdi: pointer to (u)int16_t
	// si: value
	sfence
	movw %si, (%rdi)
	ret


.global _ZN2nx6atomic5storeEPii; .type _ZN2nx6atomic5storeEPii, @function
.global _ZN2nx6atomic5storeEPjj; .type _ZN2nx6atomic5storeEPjj, @function
_ZN2nx6atomic5storeEPii:
_ZN2nx6atomic5storeEPjj:
	// rdi: pointer to (u)int32_t
	// esi: value
	sfence
	movl %esi, (%rdi)
	ret


.global _ZN2nx6atomic5storeEPll; .type _ZN2nx6atomic5storeEPll, @function
.global _ZN2nx6atomic5storeEPmm; .type _ZN2nx6atomic5storeEPmm, @function
_ZN2nx6atomic5storeEPll:
_ZN2nx6atomic5storeEPmm:
	// rdi: pointer to (u)int64_t
	// rsi: value
	sfence
	movq %rsi, (%rdi)
	ret


.global _ZN2nx6atomic5storeEPvS1_; .type _ZN2nx6atomic5storeEPvS1_, @function
_ZN2nx6atomic5storeEPvS1_:
	// rdi: void**
	// rsi: void*
	sfence
	movq %rsi, (%rdi)
	ret
