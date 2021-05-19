	.file	1 "gen_pthread-pi-defines.c"
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
 # 4 "libpthread/nptl/sysdeps/unix/sysv/linux/gen_pthread-pi-defines.c" 1
	@@@name@@@MUTEX_KIND@@@value@@@12@@@end@@@
 # 0 "" 2
 # 5 "libpthread/nptl/sysdeps/unix/sysv/linux/gen_pthread-pi-defines.c" 1
	@@@name@@@ROBUST_BIT@@@value@@@16@@@end@@@
 # 0 "" 2
 # 6 "libpthread/nptl/sysdeps/unix/sysv/linux/gen_pthread-pi-defines.c" 1
	@@@name@@@PI_BIT@@@value@@@32@@@end@@@
 # 0 "" 2
 # 7 "libpthread/nptl/sysdeps/unix/sysv/linux/gen_pthread-pi-defines.c" 1
	@@@name@@@PS_BIT@@@value@@@128@@@end@@@
 # 0 "" 2
#NO_APP
	j	$31
	.end	dummy
	.size	dummy, .-dummy
	.ident	"GCC: (Buildroot 2012.11.1) 4.6.3"
