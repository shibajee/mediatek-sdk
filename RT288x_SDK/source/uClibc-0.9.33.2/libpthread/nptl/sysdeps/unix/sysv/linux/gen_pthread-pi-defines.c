#include <pthreadP.h>
void dummy(void);
void dummy(void) {
__asm__ ("@@@name@@@MUTEX_KIND@@@value@@@%0@@@end@@@" : : "i" ((long) offsetof (pthread_mutex_t, __data.__kind)));
__asm__ ("@@@name@@@ROBUST_BIT@@@value@@@%0@@@end@@@" : : "i" ((long) PTHREAD_MUTEX_ROBUST_NORMAL_NP));
__asm__ ("@@@name@@@PI_BIT@@@value@@@%0@@@end@@@" : : "i" ((long) PTHREAD_MUTEX_PRIO_INHERIT_NP));
__asm__ ("@@@name@@@PS_BIT@@@value@@@%0@@@end@@@" : : "i" ((long) PTHREAD_MUTEX_PSHARED_BIT));
}
