// oob-server.cc: 接收带外数据
#include "inet_socket.h"
#include "util/errors.hpp"
#include <fcntl.h>
#include <signal.h>

using namespace socket_util;
using namespace socket_util::util;

static volatile bool is_urg_data = false;

void sigurg_handler(int sig) {
    if (sig == SIGURG) {
        is_urg_data = true;
    }
}

int main() {
    int sockfd = inet::createTcpServer({"127.0.0.1", 8888}, false);
    error::ExitIf(sockfd == -1, errno, "createTcpServer");

    // 与客户建立连接
    int connfd = inet::accept(sockfd);
    error::ExitIf(connfd == -1, errno, "accept");

    // 对带外数据产生的信号进行处理
    if (-1 == fcntl(connfd, F_SETOWN, getpid()))
        error::Exit(errno, "fcntl %d F_SETOWN", connfd);
    signal(SIGURG, sigurg_handler);

    char buffer[1024];

    for (size_t i = 0; ; ++i) {
        int flags = (is_urg_data ? MSG_OOB : 0);
        int num_recv = static_cast<int>(inet::recv(connfd, buffer, flags));
        //ssize_t num_recv = inet::recv(connfd, buffer, sizeof(buffer), flags);
        is_urg_data = false;
        if (num_recv == -1) {
            if (errno == EINTR)
                continue;
            else
                error::Exit(errno, "inet::recv()");
        }

        if (num_recv == 0)
            break;

        printf("got %d bytes of %s data '%.*s'\n",
                static_cast<int>(num_recv),
                is_urg_data ? "oob" : "normal",
                static_cast<int>(num_recv),
                buffer);

        sleep(1);
    }

    close(connfd);
    close(sockfd);
    return 0;
}
/**
~/socket_util/examples# ./oob-server &
[1] 13355
~/socket_util/examples# ./oob-client
got 3 bytes of normal data '123'
got 1 bytes of normal data 'c'
~/socket_util/examples# got 2 bytes of normal data 'ab'
got 3 bytes of normal data '123'

[1]+  Done                    ./oob-server
 */
