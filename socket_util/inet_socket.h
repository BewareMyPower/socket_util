// inet_socket.h:
#pragma once

#include <string.h>
#include <sys/socket.h>

#include <string>
#include <utility>  // std::pair

#include "inet_address.h"

namespace socket_util {

namespace inet {

// ----------------------------------------------------------------------------
// 自定义的实用函数

/**
 * 功能: 创建监听指定地址的TCP套接字
 * 参数:
 *   address 绑定&监听的地址，同InetAddress::newInstance()的address参数
 *   nonblocking 默认为true，此时为非阻塞套接字
 *   backlog 同listen(2)的参数2
 * 返回值:
 *   若成功创建则返回其套接字描述符，否则打印错误并退出程序
 */
int createTcpServer(std::string address, bool nonblocking = true,
        int backlog = SOMAXCONN) noexcept;

/**
 * 功能: 创建连接到指定地址的TCP套接字
 *   目前使用的阻塞式套接字，TODO: 修改成非阻塞式
 * 参数:
 *   address 被动连接的套接字地址，同InetAddress::newInstance()的address参数
 * 返回值:
 *   若成功创建则返回其套接字描述符，否则返回-1。
 *   不同于createTcpServer直接退出，因为客户端在连接失败时可能会采取其他措施
 */
int createTcpClient(std::string address) noexcept;

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

inline ssize_t send(int sockfd, const char* buf, size_t len, int flags = 0) noexcept {
    return ::send(sockfd, buf, len, flags);
}

inline ssize_t send(int sockfd, const char* buf, int flags = 0) noexcept {
    return inet::send(sockfd, buf, strlen(buf), flags);
}

inline ssize_t send(int sockfd, const std::string& buf, int flags = 0) noexcept {
    return inet::send(sockfd, buf.data(), buf.size(), flags);
}

inline ssize_t sendto(int sockfd, const char* buf, size_t len,
        InetAddress& addr, int flags = 0) noexcept {
    return ::sendto(sockfd, buf, len, flags, addr.getSockaddrPtr(), InetAddress::LENGTH);
}

inline ssize_t sendto(int sockfd, const char* buf, InetAddress& addr, int flags = 0) noexcept {
    return inet::sendto(sockfd, buf, strlen(buf), addr, flags);
}

inline ssize_t sendto(int sockfd, const std::string& buf, InetAddress& addr, int flags = 0) noexcept {
    return inet::sendto(sockfd, buf.data(), buf.size(), addr, flags);
}


inline ssize_t recv(int sockfd, char* buf, size_t len, int flags = 0) noexcept {
    return ::recv(sockfd, buf, len, flags);
}

inline ssize_t recvfrom(int sockfd, char* buf, size_t len,
        InetAddress& addr, int flags = 0) noexcept {
    socklen_t addrlen = InetAddress::LENGTH;
    return ::recvfrom(sockfd, buf, len, flags, addr.getSockaddrPtr(), &addrlen);
}

template <size_t N>
inline ssize_t recv(int sockfd, char (&buf)[N], int flags = 0) noexcept {
    return ::recv(sockfd, &buf[0], N, flags);
}

template <size_t N>
inline ssize_t recvfrom(int sockfd, char (&buf)[N],
        InetAddress& addr, int flags = 0) noexcept {
    return inet::recvfrom(sockfd, buf, N, addr, flags);
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

// ----------------------------------------------------------------------------
// 基于包装后的socket API实现的实用函数

// return true if all bytes of `buf[len]` were sent
inline bool sendAll(int sockfd, const char* buf, size_t len, int flags = 0) noexcept {
    size_t num_left = len;
    while (num_left > 0) {
        ssize_t num_send = inet::send(sockfd, buf, num_left, flags);
        if (num_send == -1) {
            if (errno == EINTR) {
                num_send = 0;
            } else {  // fatal error
                return false;
            }
        }

        num_left -= num_send;
        buf += num_send;
    }
    return true;
}

inline bool sendAll(int sockfd, const char* buf, int flags = 0) noexcept {
    return inet::sendAll(sockfd, buf, strlen(buf), flags);
}

inline bool sendAll(int sockfd, const std::string& buf, int flags = 0) noexcept {
    return inet::sendAll(sockfd, buf.data(), buf.size(), flags);
}

}  // END namespace socket_util::inet

}  // END namespace socket_util
