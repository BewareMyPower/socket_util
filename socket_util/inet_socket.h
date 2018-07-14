// inet_socket.h:
#pragma once

#include <sys/socket.h>
#include <utility>  // std::pair
#include "inet_address.h"

namespace socket_util {

namespace inet {

// ----------------------------------------------------------------------------
// 自定义的实用函数

// 创建一个监听到指定地址的TCP套接字, 若创建失败则返回-1，errno被设置
int createTcpServer(const InetAddress& addr,
                    bool nonblocking = true, int backlog = SOMAXCONN);

// ----------------------------------------------------------------------------
// socket底层系统调用的简单包装

inline int socket(int type = SOCK_STREAM) noexcept {
    return ::socket(AF_INET, type, 0);
}

inline bool connect(int sockfd, const InetAddress& addr) noexcept {
    return ::connect(sockfd, addr.getSockaddrPtr(), addr.LENGTH) != -1;
}

inline bool bind(int sockfd, const InetAddress& addr) noexcept {
    return ::bind(sockfd, addr.getSockaddrPtr(), addr.LENGTH) != -1;
}

inline bool listen(int sockfd, int backlog = SOMAXCONN) noexcept {
    return ::listen(sockfd, backlog) != -1;
}

inline int accept(int sockfd, InetAddress& addr) noexcept {
    socklen_t addrlen = addr.LENGTH;
    return ::accept(sockfd, addr.getSockaddrPtr(), &addrlen);
}

inline int accept(int sockfd) noexcept {
    return ::accept(sockfd, nullptr, nullptr);
}

inline InetAddress getsockname(int sockfd) noexcept {
    InetAddress addr;
    socklen_t len = addr.LENGTH;
    if (::getsockname(sockfd, addr.getSockaddrPtr(), &len) != -1) {
        return addr;
    } else {
        return {"0.0.0.0", 0};
    }
}

inline InetAddress getpeername(int sockfd) noexcept {
    InetAddress addr;
    socklen_t len = addr.LENGTH;
    if (::getpeername(sockfd, addr.getSockaddrPtr(), &len) != -1) {
        return addr;
    } else {
        return {"0.0.0.0", 0};
    }
}

template <typename OptType>
inline bool setsockopt(int sockfd, int level, int optname, const OptType& optval) noexcept {
    return ::setsockopt(sockfd, level, optname, &optval, sizeof(optval)) != -1;
}

template <typename OptType>
inline std::pair<OptType, bool> getsockopt(int sockfd, int level, int optname) noexcept {
    OptType optval;
    socklen_t optlen = sizeof(optval);
    int ret = ::getsockopt(sockfd, level, optname, &optval, &optlen);

    return std::make_pair(optval, ret != -1);
}

}  // END namespace socket_util::inet

}  // END namespace socket_util
