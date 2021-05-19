#include <limits.h>
#include <stddef.h>
#include <sched.h>
#include <bits/pthreadtypes.h>
#include "internaltypes.h"
void dummy(void);
void dummy(void) {
__asm__ ("@@@name@@@VALUE@@@value@@@%0@@@end@@@" : : "i" ((long) offsetof (struct new_sem, value)));
__asm__ ("@@@name@@@PRIVATE@@@value@@@%0@@@end@@@" : : "i" ((long) offsetof (struct new_sem, private)));
__asm__ ("@@@name@@@NWAITERS@@@value@@@%0@@@end@@@" : : "i" ((long) offsetof (struct new_sem, nwaiters)));
__asm__ ("@@@name@@@SEM_VALUE_MAX@@@value@@@%0@@@end@@@" : : "i" ((long) SEM_VALUE_MAX));
}
