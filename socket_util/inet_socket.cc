// inet_socket.cc
#include "inet_socket.h"
#include <fcntl.h>
#include "helpers/errors.hpp"

namespace socket_util {

namespace inet {

int createTcpServer(std::string address, bool nonblocking, int backlog) noexcept {
    int sockfd = inet::socket();
    error::ExitIf(sockfd == -1, errno, "socket");

    if (nonblocking) {
        int flags = fcntl(sockfd, F_GETFL, 0);
        error::ExitIf(flags == -1, errno, "fcntl GETFL");

        flags |= O_NONBLOCK;

        if (fcntl(sockfd, F_SETFL, 0) == -1)
            error::Exit(errno, "fcntl SETFL %d", flags);
    }

    if (!inet::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, 1))
        error::Exit(errno, "setsockopt SOL_SOCKET SO_REUSEADDR 1");

    auto&& inet_address = InetAddress::newInstance(address);
    if (!inet::bind(sockfd, inet_address))
        error::Exit(errno, "bind");

    if (!inet::listen(sockfd, backlog))
        error::Exit("listen %d", backlog);

    return sockfd;
}

int createTcpClient(std::string address) noexcept {
    int sockfd = inet::socket();
    if (sockfd == -1)
        return -1;

    auto&& inet_address = InetAddress::newInstance(address);
    if (!inet::connect(sockfd, inet_address)) {
        ::close(sockfd);
        return -1;
    }

    return sockfd;
}

}  // END namespace socket_util::inet

}  // END namespace socket_util
