#include <stddef.h>
#include <sched.h>
#include <bits/pthreadtypes.h>
#include "internaltypes.h"
void dummy(void);
void dummy(void) {
__asm__ ("@@@name@@@CURR_EVENT@@@value@@@%0@@@end@@@" : : "i" ((long) offsetof (struct pthread_barrier, curr_event)));
__asm__ ("@@@name@@@MUTEX@@@value@@@%0@@@end@@@" : : "i" ((long) offsetof (struct pthread_barrier, lock)));
__asm__ ("@@@name@@@LEFT@@@value@@@%0@@@end@@@" : : "i" ((long) offsetof (struct pthread_barrier, left)));
__asm__ ("@@@name@@@INIT_COUNT@@@value@@@%0@@@end@@@" : : "i" ((long) offsetof (struct pthread_barrier, init_count)));
__asm__ ("@@@name@@@PRIVATE@@@value@@@%0@@@end@@@" : : "i" ((long) offsetof (struct pthread_barrier, private)));
}
