#include <errno.h>
void dummy(void);
void dummy(void) {
__asm__ ("@@@name@@@EAGAIN@@@value@@@%0@@@end@@@" : : "i" ((long) EAGAIN));
__asm__ ("@@@name@@@EBUSY@@@value@@@%0@@@end@@@" : : "i" ((long) EBUSY));
__asm__ ("@@@name@@@EDEADLK@@@value@@@%0@@@end@@@" : : "i" ((long) EDEADLK));
__asm__ ("@@@name@@@EINTR@@@value@@@%0@@@end@@@" : : "i" ((long) EINTR));
__asm__ ("@@@name@@@EINVAL@@@value@@@%0@@@end@@@" : : "i" ((long) EINVAL));
__asm__ ("@@@name@@@ENOSYS@@@value@@@%0@@@end@@@" : : "i" ((long) ENOSYS));
__asm__ ("@@@name@@@EOVERFLOW@@@value@@@%0@@@end@@@" : : "i" ((long) EOVERFLOW));
__asm__ ("@@@name@@@ETIMEDOUT@@@value@@@%0@@@end@@@" : : "i" ((long) ETIMEDOUT));
__asm__ ("@@@name@@@EWOULDBLOCK@@@value@@@%0@@@end@@@" : : "i" ((long) EWOULDBLOCK));
}
