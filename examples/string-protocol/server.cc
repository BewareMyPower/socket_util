// server.cc
#include <socket_util.h>
using namespace socket_util;

#include "protocol.h"

int main(int argc, char* argv[]) {
    int listenfd = inet::createTcpServer("localhost:8888", false);
    int connfd = inet::accept(listenfd);
    inet::setnonblocking(connfd);

    Poller poller;
    poller.add(connfd, EPOLLIN | EPOLLRDHUP);

    protocol::String buf;
    while (true) {
        auto events = poller.poll();
        error::ExitIf(events.size() != 1, errno, "[server] Poller::poll return %zu", events.size());

        if (events[0].events & EPOLLRDHUP)
            error::Exit("[server] detect client close");

        if (!(events[0].events & EPOLLIN))
            error::Exit("[server] unexpected events: %d", events[0].events);

        if (!buf.recvFrom(connfd))
            error::Exit(errno, "[server] protocol::String::recvFromSocket");

        printf("[server] size: %u\n", buf.getLength());
        printf("[server] data: %s\n", buf.getBuffer().data());
    }

    close(connfd);
    close(listenfd);
    return 0;
}
