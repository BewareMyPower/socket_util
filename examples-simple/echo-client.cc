// echo-client.cc
#include "helper.h"

int main() {
    constexpr size_t BUF_SIZE = 1024;
    int sockfd = helper::connectServer("localhost:8888");
    printf("Input buf to send (Press Ctrl+D to quit)\n");

    char buf[BUF_SIZE];
    while (true) {
        ssize_t num_to_send = ::read(STDIN_FILENO, buf, BUF_SIZE);
        if (num_to_send == -1) {
            error::Exit(errno, "read STDIN_FILENO");
        } else if (num_to_send == 0) {
            printf("[client] closed\n");
            close(sockfd);
            break;
        } else {
            if (buf[num_to_send - 1] == '\n') {  // 去除换行符
                buf[num_to_send - 1] = '\0';
                --num_to_send;

                if (num_to_send == 0)  // 空字符串不予发送
                    continue;
            } else {
                error::Exit("No newline!");
            }

            if (!inet::send(sockfd, buf, num_to_send))
                error::Exit(errno, "inet::sendCString");

            // 从服务器接收同样数量的消息
            ssize_t num_recv = 0;
            while (num_recv < num_to_send) {
                ssize_t ret = inet::recv(sockfd, buf + num_recv, num_to_send - num_recv);
                if (ret <= 0) {
                    error::Exit(errno, "recv");
                } else {
                    num_recv += ret;
                }
            }
            buf[num_recv] = '\0';  // 由于去除了换行符，所以num_recv < BUF_SIZE
            printf("[client] recv %zd bytes: %s\n", num_recv, buf);
        }
    }
    return 0;
}
