// inet_socket.cc
#include "inet_socket.h"
#include <fcntl.h>
#include "util/fd_guard.hpp"

namespace socket_util {

namespace inet {

int createTcpServer(const InetAddress& addr, bool nonblocking, int backlog) noexcept {
    int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) return -1;

    util::FdGuard guard{sockfd};

    if (nonblocking) {
        // FIXME: 检查fcntl返回值是否必要？
        int flags = fcntl(sockfd, F_GETFL, 0);
        if (flags == -1) return -1;

        flags |= O_NONBLOCK;

        if (fcntl(sockfd, F_SETFL, flags) == -1) return -1;
    }

    if (!inet::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, 1)) return -1;
    if (!inet::bind(sockfd, addr)) return -1;
    if (!inet::listen(sockfd, backlog)) return -1;

    guard.detach();  // 套接字绑定、监听成功，不关闭套接字
    return sockfd;
}

int createTcpClient(const InetAddress& addr) noexcept {
    int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) return -1;

    util::FdGuard guard{sockfd};

    if (!inet::connect(sockfd, addr)) return -1;

    guard.detach();  // 套接字连接成功，不关闭套接字
    return sockfd;
}

}  // END namespace socket_util::inet

}  // END namespace socket_util
