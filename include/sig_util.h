#pragma once

#include <signal.h>
#include "error_functions.hpp"

using SigfuncPtr = void (*)(int);

void sig_child(int);  // SIGCHLD信号的处理器

// 包裹函数
inline SigfuncPtr Signal(int sig, SigfuncPtr func) {
    if (signal(sig, func) == SIG_ERR)
        err_sys("signal");
    return func;
}
