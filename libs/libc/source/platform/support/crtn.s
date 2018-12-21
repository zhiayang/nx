// crtn.s
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

.section .init
   /* gcc will nicely put the contents of crtend.o's .init section here. */
   popq %rbp
   ret
// .size _init, . - _init

.section .fini
   /* gcc will nicely put the contents of crtend.o's .fini section here. */
   popq %rbp
   ret
// .size _fini, . - _fini
