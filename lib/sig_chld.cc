// sig_child.cc: 对SIGCHLD信号的处理方式
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

void sig_child(int) {  // 初始版本
    int status;
    pid_t pid = wait(&status);
    printf("child %ld terminated\n", static_cast<long>(pid));
}
