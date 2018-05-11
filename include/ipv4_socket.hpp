#pragma once

#include <utility>

#include "socket.hpp"
#include "ipv4_addr.h"

class Ipv4Socket : public Socket {
public:
    explicit Ipv4Socket(int type, int protocol) : Socket(AF_INET, type, protocol) {}
    explicit Ipv4Socket(int fd) : Socket(fd) {}

    void Bind(const char* ip, uint16_t) const;
    void Bind(const std::string& ip, uint16_t port) const;
    void Connect(const char* ip, uint16_t port) const;
    void Connect(const std::string& ip, uint16_t port) const;
    Ipv4Socket Accept() const;
    std::pair<Ipv4Socket, Ipv4Addr> AcceptWithAddr() const;

    Ipv4Addr GetSockName() const;
    Ipv4Addr GetPeerName() const;

};

// 工厂方法: 支持仅含1个参数的初始化
inline Ipv4Socket CreateIpv4Socket(int type = SOCK_STREAM, int protocol = 0) {
    return Ipv4Socket(type, protocol);
}

inline void Ipv4Socket::Bind(const char* ip, uint16_t port) const {
    Ipv4Addr addr{ip, port};
    if (::bind(fd_, addr.GetPtrToSA(), sizeof(Ipv4Addr::SA_IN)) == -1)
        err_sys("bind() to %s:%d failed", ip, port);
}

inline void Ipv4Socket::Bind(const std::string& ip, uint16_t port) const {
    this->Bind(ip.c_str(), port);
}

inline void Ipv4Socket::Connect(const char* ip, uint16_t port) const {
    Ipv4Addr addr(ip, port);
    if (::connect(fd_, addr.GetPtrToSA(), sizeof(Ipv4Addr::SA_IN)) == -1)
        err_sys("connect() to %s failed", ip, port);
}

inline void Ipv4Socket::Connect(const std::string& ip, uint16_t port) const {
    this->Connect(ip.c_str(), port);
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
