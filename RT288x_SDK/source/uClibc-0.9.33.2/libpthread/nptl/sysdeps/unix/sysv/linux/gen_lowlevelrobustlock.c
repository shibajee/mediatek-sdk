#include <stddef.h>
#include <pthreadP.h>
void dummy(void);
void dummy(void) {
__asm__ ("@@@name@@@TID@@@value@@@%0@@@end@@@" : : "i" ((long) offsetof (struct pthread, tid)));
}
