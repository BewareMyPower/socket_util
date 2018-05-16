#pragma once

#include <assert.h>
#include <unistd.h>
#include <sys/socket.h>

#include "non_copyable.hpp"
#include "error_functions.hpp"

class Socket : public NonCopyable {
public:
    void Listen(int backlog = SOMAXCONN) const;

    ssize_t Send(const char* buf, size_t len, int flags = 0) const;

    ssize_t Recv(char* buf, size_t len, int flags = 0) const;

    void Close() noexcept;

    template <typename OptType>
    void SetSockOpt(int level, int optname, const OptType& optval) const;

    template <typename OptType>
    OptType GetSockOpt(int level, int optname) const;

    int GetFd() const { return fd_; }

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

protected:
    explicit Socket(int domain, int type, int protocol = 0);
    explicit Socket(int fd) : fd_(fd) {}
    virtual ~Socket();

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
    Close();
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

inline void Socket::Close() noexcept {
    if (fd_ >= 0) {
        close(fd_);
        fd_ = -1;
    }
}

inline ssize_t
Socket::RecvNBytes(char* buf, size_t n, int flags/* = 0*/) {
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
Socket::SendNBytes(const char* buf, size_t n, int flags/* = 0*/) {
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
