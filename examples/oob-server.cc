// oob-server.cc: 接收带外数据
#include "helper.h"

int main() {
    int connfd = helper::acceptOneConnect("localhost:8888");
    Poller poller;
    poller.add(connfd, EPOLLIN | EPOLLPRI);

    char buf[1024];
    while (true) {
        auto events = poller.poll();
        error::ExitIf(events.empty(), errno, "Poller::poll");

        int flags = -1;
        if (events[0].events & EPOLLIN)
            flags = 0;
        if (events[0].events & EPOLLPRI)
            flags |= MSG_OOB;

        if (flags == -1) {
            error::Println("unexpected event: %d", events[0].events);
            continue;
        }

        ssize_t num_recv = inet::recv(connfd, buf, sizeof(buf), flags);
        if (num_recv == -1) {
            error::Exit(errno, "inet::recv");
        } else if (num_recv == 0) {
            close(connfd);
            break;
        } else {
            printf("recv %s data: %.*s\n", (flags & MSG_OOB) ? "oob" : "normal",
                    static_cast<int>(num_recv), buf);
        }
    }

    return 0;
}
