// inet_socket.cc
#include <fcntl.h>
#include "inet_socket.h"
#include "poller.h"
#include "helpers/errors.hpp"

namespace socket_util {

namespace inet {

void setnonblocking(int fd) noexcept {
    int flags = fcntl(fd, F_GETFL, 0);
    error::ExitIf(flags == -1, errno, "fcntl GETFL");

    flags |= O_NONBLOCK;

    if (fcntl(fd, F_SETFL, flags) == -1)
        error::Exit(errno, "fcntl SETFL %d", flags);
}

int createTcpServer(std::string&& address, bool nonblocking, int backlog) noexcept {
    int sockfd = inet::socket();
    error::ExitIf(sockfd == -1, errno, "socket");

    if (nonblocking)
        setnonblocking(sockfd);

    if (!inet::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, 1))
        error::Exit(errno, "setsockopt SOL_SOCKET SO_REUSEADDR 1");

    auto&& inet_address = InetAddress::newInstance(std::move(address));
    if (!inet::bind(sockfd, inet_address))
        error::Exit(errno, "bind");

    if (!inet::listen(sockfd, backlog))
        error::Exit("listen %d", backlog);

    return sockfd;
}

int createTcpClient(std::string&& address) noexcept {
    int sockfd = inet::socket();
    if (sockfd == -1)
        return -1;

    auto&& inet_address = InetAddress::newInstance(std::move(address));
    if (!inet::connect(sockfd, inet_address)) {
        ::close(sockfd);
        return -1;
    }

    return sockfd;
}

bool connect(int sockfd, const InetAddress& addr, unsigned int timeout_ms) noexcept {
    int old_errno = errno;
    if (inet::connect(sockfd, addr))
        return true;

    if (errno == EINPROGRESS) {
        errno = old_errno;
    } else {
        error::Exit(errno, "inet::connect");
    }

    Poller poller;
    poller.add(sockfd, EPOLLOUT);
    auto events = poller.poll(static_cast<int>(timeout_ms));
    if (events.empty()) {
        return false;
    } else {
        // FIXME: how to handle EPOLLERR?
        return true;
    }
}

}  // END namespace socket_util::inet

}  // END namespace socket_util
