// oob-server.cc: 接收带外数据
#include "helper.h"

#include <fcntl.h>
#include <signal.h>

static volatile bool is_urg_data = false;

int main() {
    int connfd = helper::acceptOneConnect("localhost:8888");

    // 对带外数据产生的信号进行处理
    if (-1 == fcntl(connfd, F_SETOWN, getpid()))
        error::Exit(errno, "fcntl %d F_SETOWN", connfd);
    signal(SIGURG, [](int) { is_urg_data = true; });

    char buffer[1024];

    for (size_t i = 0; ; ++i) {
        int flags = (is_urg_data ? MSG_OOB : 0);
        int num_recv = static_cast<int>(inet::recv(connfd, buffer, flags));
        if (num_recv == -1) {
            if (errno == EINTR)
                continue;
            else
                error::Exit(errno, "inet::recv()");
        }

        if (num_recv == 0)
            break;

        printf("got %d bytes of %s data '%.*s'\n",
                num_recv, (is_urg_data ? "oob" : "normal"), num_recv, buffer);
        is_urg_data = false;

        // 因为不清楚带外数据到达的具体时刻，所以休眠1秒等待信号
        // 否则可能因为读取所有可读数据之后退出循环
        sleep(1);
    }

    close(connfd);
    return 0;
}
/**
~/socket_util/examples# ./oob-server &
[1] 17830
~/socket_util/examples# netstat -anpt | grep 17830
tcp        0      0 127.0.0.1:8888          0.0.0.0:*               LISTEN      17830/oob-server
~/socket_util/examples# ./oob-client
got 3 bytes of normal data '123'
got 1 bytes of oob data 'c'
~/socket_util/examples# got 2 bytes of normal data 'ab'
got 3 bytes of normal data '123'

[1]+  Done                    ./oob-server
 */
