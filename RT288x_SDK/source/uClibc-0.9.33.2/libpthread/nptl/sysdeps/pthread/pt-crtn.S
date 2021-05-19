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
/*@_init_EPILOG_BEGINS*/
 # 0 "" 2
 # 91 "libpthread/nptl/sysdeps/pthread/pt-initfini.c" 1
	.section .init
 # 0 "" 2
#NO_APP
	lw	$31,28($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,32
	.set	macro
	.set	reorder

#APP
	END_INIT
	
/*@_init_EPILOG_ENDS*/
/*@_fini_EPILOG_BEGINS*/
 # 0 "" 2
 # 120 "libpthread/nptl/sysdeps/pthread/pt-initfini.c" 1
	.section .fini
 # 0 "" 2
#NO_APP
	lw	$31,28($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,32
	.set	macro
	.set	reorder

#APP
	END_FINI
	
/*@_fini_EPILOG_ENDS*/
/*@TRAILER_BEGINS*/
	.hidden	__pthread_initialize_minimal_internal
	.ident	"GCC: (GNU) 4.3.4"
