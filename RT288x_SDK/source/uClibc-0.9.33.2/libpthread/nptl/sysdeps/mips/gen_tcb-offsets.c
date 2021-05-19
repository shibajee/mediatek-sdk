#include <sysdep.h>
#include <tls.h>
void dummy(void);
void dummy(void) {
#define thread_offsetof(mem)	(long)(offsetof(struct pthread, mem) - TLS_TCB_OFFSET - TLS_PRE_TCB_SIZE)
__asm__ ("@@@name@@@MULTIPLE_THREADS_OFFSET@@@value@@@%0@@@end@@@" : : "i" ((long) thread_offsetof (header.multiple_threads)));
__asm__ ("@@@name@@@PID_OFFSET@@@value@@@%0@@@end@@@" : : "i" ((long) thread_offsetof (pid)));
__asm__ ("@@@name@@@TID_OFFSET@@@value@@@%0@@@end@@@" : : "i" ((long) thread_offsetof (tid)));
}
