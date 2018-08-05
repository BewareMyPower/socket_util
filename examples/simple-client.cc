// simple-client.cc: 简单的从服务器中尽可能接收数据，接收完毕后断开连接
#include "helper.h"

int main() {
    int sockfd = inet::createTcpClient("localhost:8888");
    error::ExitIf(sockfd == -1, errno, "createTcpClient");

    char buf[1024];
    while (true) {
        ssize_t n = inet::recv(sockfd, buf, sizeof(buf));
        if (n > 0) {
            printf("[client] recv: %.*s\n", static_cast<int>(n), buf);
        } else if (n == 0) {
            printf("[client] EOF\n");
            break;
        } else {
            error::Exit(errno, "recv");
        }
    }
    return 0;
}
