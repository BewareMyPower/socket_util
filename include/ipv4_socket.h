#pragma once

#include <utility>

#include "socket.hpp"
#include "ipv4_addr.h"

class Ipv4Socket : public Socket {
public:
    explicit Ipv4Socket(int type, int protocol) : Socket(AF_INET, type, protocol) {}
    explicit Ipv4Socket(int fd) : Socket(fd) {}

    void Bind(const std::string& ip, uint16_t port) const;
    void Connect(const std::string& ip, uint16_t port) const;
    Ipv4Socket Accept() const;
    std::pair<Ipv4Socket, Ipv4Addr> AcceptWithAddr() const;

    Ipv4Addr GetSockName() const;
    Ipv4Addr GetPeerName() const;

    /**
     * 功能: 从套接字中读取若干个字节到缓冲区
     * 参数:
     *   buf   指向缓冲区
     *   n     读取的字节数量
     *   flags 同recv(3)的flags参数
     * 返回值:
     *   若读取成功则返回读取到的字节数，否则返回-1，errno被设置
     *   只有读到EOF时返回值才可能小于n，否则必然返回n或者-1
     *   当内部调用的recv(3)被信号中断时会重新调用recv(3)
     */
    ssize_t RecvNBytes(char* buf, size_t n, int flags = 0);

    /**
     * 功能: 从缓冲区写入若干个字节到套接字中
     * 参数:
     *   buf   指向缓冲区
     *   n     写入的字节数量
     *   flags 同send(3)的flags参数
     * 返回值:
     *   若写入成功则必然写入n个字节到套接字中，否则返回-1，errno被设置
     *   当内部调用的send(3)被信号中断时会重新调用send(3)
     */
    ssize_t SendNBytes(const char* buf, size_t n, int flags = 0);
};

// 工厂方法: 支持仅含1个参数的初始化
inline Ipv4Socket CreateIpv4Socket(int type, int protocol = 0) {
    return Ipv4Socket(type, protocol);
}

inline void Ipv4Socket::Bind(const std::string& ip, uint16_t port) const {
    Ipv4Addr addr{ip, port};
    if (::bind(fd_, addr.GetPtrToSA(), sizeof(Ipv4Addr::SA_IN)) == -1)
        err_sys("bind() to %s:%d failed", ip.c_str(), port);
}

inline void Ipv4Socket::Connect(const std::string& ip, uint16_t port) const {
    Ipv4Addr addr(ip, port);
    if (::connect(fd_, addr.GetPtrToSA(), sizeof(Ipv4Addr::SA_IN)) == -1)
        err_sys("connect() to %s failed", ip.c_str(), port);
}

inline Ipv4Socket Ipv4Socket::Accept() const {
    int connfd = ::accept(fd_, nullptr, nullptr);
    if (connfd == -1)
        err_sys("accept() failed");
    return Ipv4Socket(connfd);
}

inline std::pair<Ipv4Socket, Ipv4Addr> Ipv4Socket::AcceptWithAddr() const {
    Ipv4Addr::SA_IN addr;
    socklen_t len = sizeof(addr);
    int connfd = ::accept(fd_, reinterpret_cast<Ipv4Addr::SA*>(&addr), &len);
    if (connfd == -1)
        err_sys("accept() failed");
    return std::make_pair(Ipv4Socket(connfd), Ipv4Addr(addr));
}

inline Ipv4Addr Ipv4Socket::GetSockName() const {
    Ipv4Addr::SA_IN addr;
    socklen_t len = sizeof(addr);
    if (::getsockname(fd_, reinterpret_cast<Ipv4Addr::SA*>(&addr), &len) == -1)
        err_sys("[fd: %d] getperrname failed", fd_);
    return Ipv4Addr(addr);
}

inline Ipv4Addr Ipv4Socket::GetPeerName() const {
    Ipv4Addr::SA_IN addr;
    socklen_t len = sizeof(addr);
    if (::getpeername(fd_, reinterpret_cast<Ipv4Addr::SA*>(&addr), &len) == -1)
        err_sys("[fd: %d] getperrname failed", fd_);
    return Ipv4Addr(addr);
}

inline ssize_t
Ipv4Socket::RecvNBytes(char* buf, size_t n, int flags/* = 0*/) {
    size_t num_left = n;  // 剩余需要读的字节数

    while (num_left > 0) {
        ssize_t num_recv = this->Recv(buf, num_left, flags);
        if (num_recv == -1) {
            if (errno != EINTR)  // 若被信号中断则重新recv
                return -1;
        } else if (num_recv == 0) {  // EOF
            break;
        }

        // 更新剩余字节数和接收字节的位置
        num_left -= num_recv;
        buf += num_recv;
    }

    return n - num_left;
}

inline ssize_t
Ipv4Socket::SendNBytes(const char* buf, size_t n, int flags/* = 0*/) {
    size_t num_left = n;  // 剩余需要写的字节数

    while (num_left > 0) {
        ssize_t num_send = this->Send(buf, num_left, flags);
        if (num_send == -1) {
            if (errno != EINTR)
                return -1;
            else  // 被信号中断则重新send
                num_send = 0;
        }

        // 更新剩余字节数和接收字节的位置
        num_left -= num_send;
        buf += num_send;
    }

    return n;
}
