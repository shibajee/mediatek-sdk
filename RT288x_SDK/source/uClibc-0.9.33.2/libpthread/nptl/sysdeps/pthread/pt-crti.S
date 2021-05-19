	.section .mdebug.abi32
	.previous
	.gnu_attribute 4, 3
	.abicalls
#APP
	
#include "defs.h"
	
#if defined __i686 && defined __ASSEMBLER__
	
#undef __i686
	
#define __i686 __i686
	
#endif
	
/*@HEADER_ENDS*/
/*@_init_PROLOG_BEGINS*/
#NO_APP
	.align	2
	.type	call_initialize_minimal, @function
call_initialize_minimal:
	.set	nomips16
	.set	noreorder
	.cpload	$25
	.set	nomacro
	
	lw	$25,%got(__pthread_initialize_minimal_internal)($28)
	jr	$25
	nop

	.set	macro
	.set	reorder
#APP
	.section .init
#NO_APP
	.section	.init,"ax",@progbits
	.align	2
	.globl	_init
	.type	_init, @function
_init:
	.set	nomips16
	.set	noreorder
	.cpload	$25
	.set	reorder
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	.cprestore	16
	lw	$25,%got(call_initialize_minimal)($28)
	addiu	$25,$25,%lo(call_initialize_minimal)
	jalr	$25
	lw	$28,16($sp)
#APP
 # 86 "libpthread/nptl/sysdeps/pthread/pt-initfini.c" 1
	ALIGN
 # 0 "" 2
 # 87 "libpthread/nptl/sysdeps/pthread/pt-initfini.c" 1
	END_INIT
 # 0 "" 2
 # 89 "libpthread/nptl/sysdeps/pthread/pt-initfini.c" 1
	
/*@_init_PROLOG_ENDS*/
/*@_fini_PROLOG_BEGINS*/
	.section .fini
#NO_APP
	.section	.fini,"ax",@progbits
	.align	2
	.globl	_fini
	.type	_fini, @function
_fini:
	.set	nomips16
	.set	noreorder
	.cpload	$25
	.set	reorder
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	.cprestore	16
#APP
 # 106 "libpthread/nptl/sysdeps/pthread/pt-initfini.c" 1
	ALIGN
 # 0 "" 2
 # 107 "libpthread/nptl/sysdeps/pthread/pt-initfini.c" 1
	END_FINI
 # 0 "" 2
 # 108 "libpthread/nptl/sysdeps/pthread/pt-initfini.c" 1
	
/*@_fini_PROLOG_ENDS*/
/*@TRAILER_BEGINS*/
	.hidden	__pthread_initialize_minimal_internal
	.ident	"GCC: (GNU) 4.3.4"
