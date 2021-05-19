	.file	1 "gen_pthread-errnos.c"
	.section .mdebug.abi32
	.previous
	.gnu_attribute 4, 3
	.abicalls
	.text
	.align	2
	.globl	dummy
	.set	nomips16
	.ent	dummy
	.type	dummy, @function
dummy:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
#APP
 # 4 "libpthread/nptl/gen_pthread-errnos.c" 1
	@@@name@@@EAGAIN@@@value@@@11@@@end@@@
 # 0 "" 2
 # 5 "libpthread/nptl/gen_pthread-errnos.c" 1
	@@@name@@@EBUSY@@@value@@@16@@@end@@@
 # 0 "" 2
 # 6 "libpthread/nptl/gen_pthread-errnos.c" 1
	@@@name@@@EDEADLK@@@value@@@45@@@end@@@
 # 0 "" 2
 # 7 "libpthread/nptl/gen_pthread-errnos.c" 1
	@@@name@@@EINTR@@@value@@@4@@@end@@@
 # 0 "" 2
 # 8 "libpthread/nptl/gen_pthread-errnos.c" 1
	@@@name@@@EINVAL@@@value@@@22@@@end@@@
 # 0 "" 2
 # 9 "libpthread/nptl/gen_pthread-errnos.c" 1
	@@@name@@@ENOSYS@@@value@@@89@@@end@@@
 # 0 "" 2
 # 10 "libpthread/nptl/gen_pthread-errnos.c" 1
	@@@name@@@EOVERFLOW@@@value@@@79@@@end@@@
 # 0 "" 2
 # 11 "libpthread/nptl/gen_pthread-errnos.c" 1
	@@@name@@@ETIMEDOUT@@@value@@@145@@@end@@@
 # 0 "" 2
 # 12 "libpthread/nptl/gen_pthread-errnos.c" 1
	@@@name@@@EWOULDBLOCK@@@value@@@11@@@end@@@
 # 0 "" 2
#NO_APP
	j	$31
	.end	dummy
	.size	dummy, .-dummy
	.ident	"GCC: (Buildroot 2012.11.1) 4.6.3"
