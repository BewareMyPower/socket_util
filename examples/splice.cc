// splice.cc: 实现零拷贝回射服务器
#include "helper.h"

int main() {
    int connfd = helper::acceptOneConnect("localhost:8888");
    int pipefd[2];
    if (pipe(pipefd) == -1)
        error::Exit(errno, "pipe");

    while (true) {
        // 将connfd上流入的客户数据定向到管道中
        ssize_t num_recv = splice(connfd, nullptr, pipefd[1], nullptr,
                32768, SPLICE_F_MORE | SPLICE_F_MOVE);
        if (num_recv == -1) {
            error::Exit(errno, "splice");
        } else if (num_recv == 0) {  // EOF
            close(connfd);
            printf("[server] closed");
            break;
        } else {
            // 将管道的输出定向到connfd客户连接文件描述符
            if (splice(pipefd[0], nullptr, connfd, nullptr, 32768,
                        SPLICE_F_MORE | SPLICE_F_MOVE) == -1) {
                error::Exit(errno, "splice");
            }
        }
    }

    return 0;
}
