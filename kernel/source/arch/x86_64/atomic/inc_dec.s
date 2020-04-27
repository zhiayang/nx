// inc_dec.s
// Copyright (c) 2020, zhiayang
// Licensed under the Apache License Version 2.0.


.global _ZN2nx6atomic4incrEPa; .type _ZN2nx6atomic4incrEPa, @function
.global _ZN2nx6atomic4incrEPh; .type _ZN2nx6atomic4incrEPh, @function
_ZN2nx6atomic4incrEPa:
_ZN2nx6atomic4incrEPh:
	// rdi: pointer to (u)int8_t
	sfence
	lock incb (%rdi)
	ret


.global _ZN2nx6atomic4incrEPs; .type _ZN2nx6atomic4incrEPs, @function
.global _ZN2nx6atomic4incrEPt; .type _ZN2nx6atomic4incrEPt, @function
_ZN2nx6atomic4incrEPs:
_ZN2nx6atomic4incrEPt:
	// rdi: pointer to (u)int16_t
	sfence
	lock incw (%rdi)
	ret


.global _ZN2nx6atomic4incrEPi; .type _ZN2nx6atomic4incrEPi, @function
.global _ZN2nx6atomic4incrEPj; .type _ZN2nx6atomic4incrEPj, @function
_ZN2nx6atomic4incrEPi:
_ZN2nx6atomic4incrEPj:
	// rdi: pointer to (u)int32_t
	sfence
	lock incl (%rdi)
	ret


.global _ZN2nx6atomic4incrEPl; .type _ZN2nx6atomic4incrEPl, @function
.global _ZN2nx6atomic4incrEPm; .type _ZN2nx6atomic4incrEPm, @function
_ZN2nx6atomic4incrEPl:
_ZN2nx6atomic4incrEPm:
	// rdi: pointer to (u)int64_t
	sfence
	lock incq (%rdi)
	ret









.global _ZN2nx6atomic4decrEPa; .type _ZN2nx6atomic4decrEPa, @function
.global _ZN2nx6atomic4decrEPh; .type _ZN2nx6atomic4decrEPh, @function
_ZN2nx6atomic4decrEPa:
_ZN2nx6atomic4decrEPh:
	// rdi: pointer to (u)int8_t
	sfence
	lock decb (%rdi)
	ret


.global _ZN2nx6atomic4decrEPs; .type _ZN2nx6atomic4decrEPs, @function
.global _ZN2nx6atomic4decrEPt; .type _ZN2nx6atomic4decrEPt, @function
_ZN2nx6atomic4decrEPs:
_ZN2nx6atomic4decrEPt:
	// rdi: pointer to (u)int16_t
	sfence
	lock decw (%rdi)
	ret


.global _ZN2nx6atomic4decrEPi; .type _ZN2nx6atomic4decrEPi, @function
.global _ZN2nx6atomic4decrEPj; .type _ZN2nx6atomic4decrEPj, @function
_ZN2nx6atomic4decrEPi:
_ZN2nx6atomic4decrEPj:
	// rdi: pointer to (u)int32_t
	sfence
	lock decl (%rdi)
	ret


.global _ZN2nx6atomic4decrEPl; .type _ZN2nx6atomic4decrEPl, @function
.global _ZN2nx6atomic4decrEPm; .type _ZN2nx6atomic4decrEPm, @function
_ZN2nx6atomic4decrEPl:
_ZN2nx6atomic4decrEPm:
	// rdi: pointer to (u)int64_t
	sfence
	lock decq (%rdi)
	ret
