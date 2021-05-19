#include <stddef.h>
#include <sched.h>
#include <bits/pthreadtypes.h>
#include <internaltypes.h>
void dummy(void);
void dummy(void) {
__asm__ ("@@@name@@@cond_lock@@@value@@@%0@@@end@@@" : : "i" ((long) offsetof (pthread_cond_t, __data.__lock)));
__asm__ ("@@@name@@@cond_futex@@@value@@@%0@@@end@@@" : : "i" ((long) offsetof (pthread_cond_t, __data.__futex)));
__asm__ ("@@@name@@@cond_nwaiters@@@value@@@%0@@@end@@@" : : "i" ((long) offsetof (pthread_cond_t, __data.__nwaiters)));
__asm__ ("@@@name@@@total_seq@@@value@@@%0@@@end@@@" : : "i" ((long) offsetof (pthread_cond_t, __data.__total_seq)));
__asm__ ("@@@name@@@wakeup_seq@@@value@@@%0@@@end@@@" : : "i" ((long) offsetof (pthread_cond_t, __data.__wakeup_seq)));
__asm__ ("@@@name@@@woken_seq@@@value@@@%0@@@end@@@" : : "i" ((long) offsetof (pthread_cond_t, __data.__woken_seq)));
__asm__ ("@@@name@@@dep_mutex@@@value@@@%0@@@end@@@" : : "i" ((long) offsetof (pthread_cond_t, __data.__mutex)));
__asm__ ("@@@name@@@broadcast_seq@@@value@@@%0@@@end@@@" : : "i" ((long) offsetof (pthread_cond_t, __data.__broadcast_seq)));
__asm__ ("@@@name@@@nwaiters_shift@@@value@@@%0@@@end@@@" : : "i" ((long) COND_NWAITERS_SHIFT));
}
