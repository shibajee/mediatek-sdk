#include <stddef.h>
#include <stdio.h>
#include <bits/pthreadtypes.h>
#include <bits/wordsize.h>
void dummy(void);
void dummy(void) {
__asm__ ("@@@name@@@MUTEX@@@value@@@%0@@@end@@@" : : "i" ((long) offsetof (pthread_rwlock_t, __data.__lock)));
__asm__ ("@@@name@@@NR_READERS@@@value@@@%0@@@end@@@" : : "i" ((long) offsetof (pthread_rwlock_t, __data.__nr_readers)));
__asm__ ("@@@name@@@READERS_WAKEUP@@@value@@@%0@@@end@@@" : : "i" ((long) offsetof (pthread_rwlock_t, __data.__readers_wakeup)));
__asm__ ("@@@name@@@WRITERS_WAKEUP@@@value@@@%0@@@end@@@" : : "i" ((long) offsetof (pthread_rwlock_t, __data.__writer_wakeup)));
__asm__ ("@@@name@@@READERS_QUEUED@@@value@@@%0@@@end@@@" : : "i" ((long) offsetof (pthread_rwlock_t, __data.__nr_readers_queued)));
__asm__ ("@@@name@@@WRITERS_QUEUED@@@value@@@%0@@@end@@@" : : "i" ((long) offsetof (pthread_rwlock_t, __data.__nr_writers_queued)));
__asm__ ("@@@name@@@FLAGS@@@value@@@%0@@@end@@@" : : "i" ((long) offsetof (pthread_rwlock_t, __data.__flags)));
__asm__ ("@@@name@@@WRITER@@@value@@@%0@@@end@@@" : : "i" ((long) offsetof (pthread_rwlock_t, __data.__writer)));
__asm__ ("@@@name@@@PSHARED@@@value@@@%0@@@end@@@" : : "i" ((long) offsetof (pthread_rwlock_t, __data.__shared)));
}
