#include <pthread.h>
#include <stddef.h>
void dummy(void);
void dummy(void) {
__asm__ ("@@@name@@@UNWINDBUFSIZE@@@value@@@%0@@@end@@@" : : "i" ((long) sizeof (__pthread_unwind_buf_t)));
__asm__ ("@@@name@@@UWJMPBUF@@@value@@@%0@@@end@@@" : : "i" ((long) offsetof (__pthread_unwind_buf_t, __cancel_jmp_buf)));
}
