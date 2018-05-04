#pragma once

#include <assert.h>
#include <unistd.h>
#include <sys/socket.h>

#include "error_functions.h"

class Socket {
public:
    void Listen(int backlog = SOMAXCONN) const;

    ssize_t Send(const char* buf, size_t len, int flags = 0) const;

    ssize_t Recv(char* buf, size_t len, int flags = 0) const;

    void Close();

    template <typename OptType>
    void SetSockOpt(int level, int optname, const OptType& optval) const;

    template <typename OptType>
    OptType GetSockOpt(int level, int optname) const;

    int GetFd() const { return fd_; }

    static constexpr size_t BUFSIZE = 1024;  // 默认recv/send缓冲区大小
protected:
    explicit Socket(int domain, int type, int protocol = 0);
    explicit Socket(int fd) : fd_(fd) {}

    virtual ~Socket();
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    Socket(Socket&&) noexcept;
    Socket& operator=(Socket&&) noexcept;

protected:
    int fd_;
};

Socket::Socket(int domain, int type, int protocol/* = 0*/) {
    fd_ = socket(domain, type, protocol);
    if (fd_ == -1)
        err_sys("socket");
}

Socket::~Socket() {
    Close();
}

Socket::Socket(Socket&& rhs) noexcept : fd_(rhs.fd_) {
    rhs.fd_ = -1;
}

Socket& Socket::operator=(Socket&& rhs) noexcept {
    assert(this != &rhs);
    fd_ = rhs.fd_;
    rhs.fd_ = -1;
    return *this;
}

inline void Socket::Listen(int backlog/* = SOMAXCONN*/) const {
    if (::listen(fd_, backlog) == -1)
        err_sys("listen");
}

inline ssize_t Socket::Send(const char* buf, size_t len, int flags/* = 0*/) const {
    return ::send(fd_, buf, len, flags);
}

inline ssize_t Socket::Recv(char* buf, size_t len, int flags/* = 0*/) const {
    return ::recv(fd_, buf, len, flags);
}

template <typename OptType> inline
void Socket::SetSockOpt(int level, int optname, const OptType& optval) const {
    if (::setsockopt(fd_, level, optname, &optval, sizeof(optval)) == -1)
        err_sys("setsockopt");
}

template <typename OptType> inline
OptType Socket::GetSockOpt(int level, int optname) const {
    OptType optval;
    socklen_t optlen = sizeof(optval);
    if (::getsockopt(fd_, level, optname, &optval, &optlen) == -1)
        err_sys("getsockopt");
    return optval;
}

inline void Socket::Close() {
    if (fd_ >= 0) {
        close(fd_);
        fd_ = -1;
    }
}

