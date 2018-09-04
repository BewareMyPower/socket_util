// inet_socket.h:
#pragma once

#include <string.h>
#include <sys/socket.h>

#include <string>
#include <utility>  // std::pair

#include "inet_address.h"
#include "io_util.h"

#define SOCKET_UTIL_INET_BEGIN namespace socket_util { namespace inet {
#define SOCKET_UTIL_INET_END }}

SOCKET_UTIL_INET_BEGIN

// ----------------------------------------------------------------------------
// 自定义的实用函数

/**
 * 功能: 设置描述符为非阻塞模式
 * 参数:
 *   fd 文件描述符
 * 说明:
 *   若设置失败，则报错并退出程序
 */
void setnonblocking(int fd) noexcept;

/**
 * 功能: 创建监听指定地址的TCP套接字
 * 参数:
 *   address 绑定&监听的地址，同InetAddress::newInstance()的address参数
 *   nonblocking 默认为true，此时为非阻塞套接字
 *   backlog 同listen(2)的参数2
 * 返回值:
 *   若成功创建则返回其套接字描述符，否则打印错误并退出程序
 */
int createTcpServer(std::string&& address, bool nonblocking = true,
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
int createTcpClient(std::string&& address) noexcept;

/**
 * 功能: 在超时时间内连接到指定套接字
 * 参数:
 *   sockfd     目的套接字描述符
 *   addr       远程套接字地址
 *   timeout_ms 超时时间，单位: 毫秒
 * 返回值:
 *   若在超时时间内连接成功则返回true，否则返回false。
 *   若出现致命性错误，打印错误并退出程序。
 */
bool connect(int sockfd, const InetAddress& addr, unsigned int timeout_ms) noexcept;

inline bool connect(int sockfd, std::string&& addr, unsigned int timeout_ms) noexcept {
    return inet::connect(sockfd, InetAddress::newInstance(std::move(addr)), timeout_ms);
}

inline bool sendAll(int sockfd, const void* buf, size_t n, int flags = 0) noexcept {
    using namespace std::placeholders;

    auto mysend = std::bind(::send, _1, _2, _3, flags);
    return io_util::writeAll(mysend, sockfd, buf, n);
}

inline bool sendCString(int sockfd, const char* str, int flags = 0) noexcept {
    return inet::sendAll(sockfd, str, strlen(str), flags);
}

inline bool sendStdString(int sockfd, const std::string& str, int flags = 0) noexcept {
    return inet::sendAll(sockfd, str.data(), str.size(), flags);
}

inline bool recvNBytes(int sockfd, void* buf, size_t n, int flags = 0) noexcept {
    using namespace std::placeholders;

    auto myrecv = std::bind(::recv, _1, _2, _3, flags);
    return io_util::readNBytes(myrecv, sockfd, buf, n);
}

// 用于非阻塞套接字，否则当可用字节数量不够时可能导致阻塞
inline ssize_t recvAsMuchAsPossible(int sockfd, void* buf, size_t n, int flags = 0) noexcept {
    using namespace std::placeholders;

    auto myrecv = std::bind(::recv, _1, _2, _3, flags);
    return io_util::readAsMuchAsPossible(myrecv, sockfd, buf, n);
}

// ----------------------------------------------------------------------------
// socket底层系统调用的简单包装

inline int socket(int type = SOCK_STREAM) noexcept {
    return ::socket(AF_INET, type, 0);
}

inline bool connect(int sockfd, const InetAddress& addr) noexcept {
    return ::connect(sockfd, addr.getSockaddrPtr(), addr.LENGTH) != -1;
}

inline bool connect(int sockfd, std::string&& addr) noexcept {
    return inet::connect(sockfd, InetAddress::newInstance(std::move(addr)));
}

inline bool bind(int sockfd, const InetAddress& addr) noexcept {
    return ::bind(sockfd, addr.getSockaddrPtr(), addr.LENGTH) != -1;
}

inline bool bind(int sockfd, std::string&& addr) noexcept {
    return inet::bind(sockfd, InetAddress::newInstance(std::move(addr)));
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

inline ssize_t sendto(int sockfd, const char* buf, size_t len,
        InetAddress& addr, int flags = 0) noexcept {
    return ::sendto(sockfd, buf, len, flags, addr.getSockaddrPtr(), InetAddress::LENGTH);
}

inline ssize_t recv(int sockfd, char* buf, size_t len, int flags = 0) noexcept {
    return ::recv(sockfd, buf, len, flags);
}

inline ssize_t recvfrom(int sockfd, char* buf, size_t len,
        InetAddress& addr, int flags = 0) noexcept {
    socklen_t addrlen = InetAddress::LENGTH;
    return ::recvfrom(sockfd, buf, len, flags, addr.getSockaddrPtr(), &addrlen);
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

SOCKET_UTIL_INET_END
