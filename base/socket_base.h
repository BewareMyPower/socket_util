// socket_base.hpp
#pragma once

#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <string>
#include <utility>  // std::pair

#include "noncopyable.hpp"

namespace socket_util {

class SocketBase : noncopyable {
public:
    SocketBase(SocketBase&& rhs) noexcept : fd_(rhs.fd_) {
        rhs.fd_ = -1;
    }

    SocketBase& operator=(SocketBase&& rhs) noexcept {
        Close();
        fd_ = rhs.fd_;
        rhs.fd_ = -1;
        return *this;
    }

    /* socket API的简单包装，返回false即调用错误，等同于原API返回-1 */

    bool Listen(int backlog = 128) const noexcept {
        return ::listen(fd_, backlog) != -1;
    }

    bool Close() noexcept {
        if (fd_ < 0) return false;
        int ret = ::close(fd_);
        fd_ = -1;
        return ret != -1;
    } 
    
    ssize_t Send(const char* buf, size_t len, int flags = 0) const noexcept {
        return ::send(fd_, buf, len, flags);
    }

    ssize_t Recv(char* buf, size_t len, int flags = 0) const noexcept {
        return ::recv(fd_, buf, len, flags);
    }

    template <typename OptType>
    bool SetSockOpt(int level, int optname, const OptType& optval) const noexcept {
        return ::setsockopt(fd_, level, optname, &optval, sizeof(optval)) != -1;
    }

    template <typename OptType>
    std::pair<bool, OptType> GetSockOpt(int level, int optname) const noexcept {
        OptType optval;
        socklen_t optlen = sizeof(optval);
        int ret = ::getsockopt(fd_, level, optname, &optval, &optlen);
        return std::make_pair(ret, optval);
    }

    /** 针对socket的实用函数 */
    // 取得内部文件描述符
    int GetFd() const noexcept { return fd_; }

    // 更换绑定的文件描述符
    void SetFd(int newfd) noexcept { fd_ = newfd; }

    // 若一次send不够或者被信号中断则重新send直到发送完n个字节为止
    // 若发送失败，返回false，errno被设置
    bool SendNBytes(const char* buf, size_t n, int flags = 0) const noexcept;

    bool SendString(const char* buf, int flags = 0) const noexcept {
        return SendNBytes(buf, strlen(buf), flags);
    }

    bool SendString(const std::string& str) const noexcept {
        return SendString(str.data(), str.size());
    }
    
    // 若doit为true，设置套接字为非阻塞模式，否则取消套接字的非阻塞位
    // 若设置/取消失败则返回false，errno被设置
    bool SetNonblocking(bool doit = true) const noexcept;

protected:
    /* 禁止直接构造该基类 */
    SocketBase() = default;
    int fd_ = -1;  // 内部套接字描述符
};

}
