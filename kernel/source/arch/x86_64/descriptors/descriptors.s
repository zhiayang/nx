// misc.s
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

.global _ZN2nx8platform8load_idtEm
.type _ZN2nx8platform8load_idtEm, @function
_ZN2nx8platform8load_idtEm:
	lidt (%rdi)
	ret

.global _ZN2nx8platform8load_gdtEm
.type _ZN2nx8platform8load_gdtEm, @function
_ZN2nx8platform8load_gdtEm:
	lgdt (%rdi)
	ret

.global _ZN2nx8platform8load_tssEt
.type _ZN2nx8platform8load_tssEt, @function
_ZN2nx8platform8load_tssEt:
	ltr %di
	ret

.global _ZN2nx8platform10get_gsbaseEv
.type _ZN2nx8platform10get_gsbaseEv, @function
_ZN2nx8platform10get_gsbaseEv:
	movq %gs:0, %rax
	ret




.global _ZN2nx8platform8rdfsbaseEv
.type _ZN2nx8platform8rdfsbaseEv, @function
_ZN2nx8platform8rdfsbaseEv:
	rdfsbase %rax
	ret

.global _ZN2nx8platform8wrfsbaseEm
.type _ZN2nx8platform8wrfsbaseEm, @function
_ZN2nx8platform8wrfsbaseEm:
	wrfsbase %rdi
	ret






