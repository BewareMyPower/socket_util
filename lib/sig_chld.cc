// sig_child.cc: 对SIGCHLD信号的处理方式
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

void sig_child(int) {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
        printf("child %ld terminated\n", static_cast<long>(pid));
}
