#include <signal.h>

using SigfuncPtr = void (*)(int);

// signal函数的POSIX语义版本
SigfuncPtr signal(int signo, SigfuncPtr func) {
    struct sigaction act;

    act.sa_handler = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
        act.sa_flags |= SA_INTERRUPT;
#endif
    } else {
#ifdef SA_RESTART
        act.sa_flags |= SA_RESTART;
#endif
    }

    struct sigaction old_act;
    if (sigaction(signo, &act, &old_act) == -1)
        return SIG_ERR;
    return old_act.sa_handler;
}
