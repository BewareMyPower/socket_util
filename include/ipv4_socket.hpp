#pragma once

#include <utility>

#include "socket.hpp"
#include "ipv4_addr.hpp"

class Ipv4Socket : public Socket {
public:
    explicit Ipv4Socket() : Socket(AF_INET, SOCK_STREAM, 0) {}
    explicit Ipv4Socket(int type, int protocol) : Socket(AF_INET, type, protocol) {}
    explicit Ipv4Socket(int fd) : Socket(fd) {}

    /** 工厂方法，来避免构造函数的重载造成的歧义
     * Create    调用socket创建相应套接字
     * AttachFd  从现有的文件描述符作为当前套接字
     * Listener  创建一个绑定本地地址并进行监听的套接字
     */
    static Ipv4Socket Create(int type = SOCK_STREAM, int protocol = 0)
        { return Ipv4Socket(type, protocol); }
    static Ipv4Socket AttachFd(int fd) { return Ipv4Socket(fd); }
    static Ipv4Socket Listener(int type, int protocol,
                               const char* ip, uint16_t port,
                               int backlog = SOMAXCONN);

    static Ipv4Socket Listener(uint16_t port, int backlog = SOMAXCONN)
        { return Listener(SOCK_STREAM, 0, "0.0.0.0", port, backlog); }

    static Ipv4Socket Listener(const char* ip, uint16_t port,
                               int backlog = SOMAXCONN)
        { return Listener(SOCK_STREAM, 0, ip, port, backlog); }

    static Ipv4Socket Listener(int type, int protocol, uint16_t port,
                               int backlog = SOMAXCONN)
        { return Listener(type, protocol, "0.0.0.0", port, backlog); }

    void Bind(const char* ip, uint16_t) const;
    void Bind(const std::string& ip, uint16_t port) const;
    void Connect(const char* ip, uint16_t port) const;
    void Connect(const std::string& ip, uint16_t port) const;
    Ipv4Socket Accept() const;
    std::pair<Ipv4Socket, Ipv4Addr> AcceptWithAddr() const;

    Ipv4Addr GetSockName() const;
    Ipv4Addr GetPeerName() const;
};

inline Ipv4Socket Ipv4Socket::Listener(int type, int protocol,
                                       const char* ip, uint16_t port,
                                       int backlog/* = SOMAXCONN*/) {
    Ipv4Socket listener(type, protocol);
    listener.SetSockOpt(SOL_SOCKET, SO_REUSEADDR, 1);
    listener.Bind(ip, port);
    listener.Listen(backlog);
    return listener;
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
    int connfd;
    while ((connfd = ::accept(fd_, nullptr, nullptr)) == -1) {
        if (errno == EINTR)  // accept被信号中断时自动重启
            continue;
        else
            err_sys("accept() failed");
    }
    return Ipv4Socket(connfd);
}

inline std::pair<Ipv4Socket, Ipv4Addr> Ipv4Socket::AcceptWithAddr() const {
    Ipv4Addr::SA_IN addr;
    auto pAddr = reinterpret_cast<Ipv4Addr::SA*>(&addr);
    socklen_t len = sizeof(addr);
    int connfd;

    while ((connfd = ::accept(fd_, pAddr, &len)) == -1) {
        if (errno == EINTR)  // accept被信号中断时自动重启
            continue;
        else
            err_sys("accept() failed");
    }
    return std::make_pair(Ipv4Socket(connfd), Ipv4Addr(addr));
}

inline Ipv4Addr Ipv4Socket::GetSockName() const {
    Ipv4Addr::SA_IN addr;
    socklen_t len = sizeof(addr);
    if (::getsockname(fd_, reinterpret_cast<Ipv4Addr::SA*>(&addr), &len) == -1)
        err_sys("[fd: %d] getsockname() failed", fd_);
    return Ipv4Addr(addr);
}

inline Ipv4Addr Ipv4Socket::GetPeerName() const {
    Ipv4Addr::SA_IN addr;
    socklen_t len = sizeof(addr);
    if (::getpeername(fd_, reinterpret_cast<Ipv4Addr::SA*>(&addr), &len) == -1)
        err_sys("[fd: %d] getpeername() failed", fd_);
    return Ipv4Addr(addr);
}
