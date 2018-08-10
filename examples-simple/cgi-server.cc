// cgi-server.cc
#include "helper.h"

int main() {
    int connfd = helper::acceptOneConnect("localhost:8888");
    if (dup2(connfd, STDOUT_FILENO) == -1)
        error::Exit("dup2()");

    // 往标准输入流写入的信息被重定向到connfd的对端
    printf("hello\n");
    inet::send(connfd, "hello", sizeof("hello"));

    close(connfd);
    return 0;
}
