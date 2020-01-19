// misc.s
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

.global nx_x64_loadidt
.type nx_x64_loadidt, @function
nx_x64_loadidt:
	lidt (%rdi)
	ret


.global nx_x64_loadgdt
.type nx_x64_loadgdt, @function
nx_x64_loadgdt:
	lgdt (%rdi)
	ret


.global nx_x64_loadtss
.type nx_x64_loadtss, @function
nx_x64_loadtss:
	ltr %di
	ret

.global nx_x64_get_gs_base
.type nx_x64_get_gs_base, @function
nx_x64_get_gs_base:
	movq %gs:0, %rax
	ret





