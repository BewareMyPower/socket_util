#pragma once
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <vector>  // 用作缓冲区
#include <string>

#include "non_copyable.hpp"

class BufferedReader : public NonCopyable {
public:
    explicit BufferedReader(int fd, size_t bufsize = 4096) : fd_(fd), buf_(bufsize) {}
    ~BufferedReader() = default;
    BufferedReader(BufferedReader&&) noexcept;
    BufferedReader& operator=(BufferedReader&&) noexcept;

    size_t GetLeftCnt() const { return leftcnt_; }

    /**
     * 功能: 读取min(n, bufsize)个字节到用户缓冲区中
     * 参数:
     *   usrbuf 指向用户缓冲区
     *   n      最多读取的字节数
     *   flags  同recv(2)的flags参数
     * 返回值:
     *   若读取成功则返回读取到的字节数，否则返回-1，errno被设置
     *   若内部调用的recv(2)被信号中断则重新调用recv(2)
     */
    ssize_t Read(char* usrbuf, size_t n, int flags = 0);

    /**
     * 功能: 读取n个字节到用户缓冲区中
     * 参数: 同类成员函数Read
     * 返回值:
     *   当内部调用的recv(2)被信号中断时会重新调用recv(2)，直到读取n个字节为止
     *   若在读到n个字节之前读取到了EOF，则会返回成功读取的字节数(<n)
     *   若读取失败则返回-1，errno被设置
     */
    ssize_t ReadNBytes(char* usrbuf, size_t n, int flags = 0);

    /**
     * 功能: 读取一行(行结束符为'\n')字节到用户缓冲区中
     * 参数:
     *   usrbuf 指向用户缓冲区
     *   maxlen 用户缓冲区的最大长度
     *   flags  同recv(2)
     * 返回值:
     *   若内部调用的recv(2)被信号中断则重新调用recv(2)，直到读取n-1个字节为止，
     *   因为要在读取的字节末尾添加'\0'作为字符串的结束符
     *   若读到行结束符'\n'或者读取n-1个字节或者读到EOF时返回，返回值为成功读取的字节数
     *   若读取失败则返回-1，errno被设置
     */
    ssize_t ReadLine(char* usrbuf, size_t maxlen, int flags = 0);

    /** 取得缓冲区中未读取的字节组成的字符串 */
    std::string GetUnreadBuffer() const;

private:
    int fd_;  // 用于I/O的文件描述符
    std::vector<char> buf_;  // 内部缓冲区
    size_t curpos_ = 0;  // 内部缓冲区中第一个未读取字节的位置
    ssize_t leftcnt_ = 0;  // 内部缓冲区中剩余未读取的字节数量
};

BufferedReader::BufferedReader(BufferedReader&& rhs) noexcept
    : fd_(rhs.fd_), buf_(std::move(rhs.buf_)) {
    rhs.fd_ = -1;
}

BufferedReader& BufferedReader::operator=(BufferedReader&& rhs) noexcept {
    assert(this != &rhs);
    fd_ = rhs.fd_;
    buf_ = std::move(rhs.buf_);
    rhs.fd_ = -1;
    return *this;
}

ssize_t BufferedReader::Read(char* usrbuf, size_t n, int flags/* = 0*/) {
    while (leftcnt_ <= 0) {  // 内部缓冲区无可读数据
        leftcnt_ = recv(fd_, buf_.data(), buf_.size(), flags);
        if (leftcnt_ < 0) {
            if (errno != EINTR)  // 遇到信号中断错误时会重新调用recv
                return -1;
        } else if (leftcnt_ == 0) {  // EOF
            return 0;
        } else {  // recv成功，重置当前位置
            curpos_ = 0;  
        }
    }

    // 从内部缓冲区拷贝min(n, leftcnt_)个字节到用户缓冲区
    size_t real_cnt = (static_cast<size_t>(leftcnt_) < n) ? leftcnt_ : n;
    memcpy(usrbuf, buf_.data() + curpos_, real_cnt);
    curpos_ += real_cnt;
    leftcnt_ -= real_cnt;
    return real_cnt;
}

ssize_t BufferedReader::ReadNBytes(char* usrbuf, size_t n, int flags/* = 0*/) {
    size_t num_left = n;
    char* ptr = usrbuf;

    while (num_left > 0) {
        ssize_t num_read = this->Read(ptr, num_left, flags);
        if (num_read == -1)
            return -1;
        else if (num_read == 0)  // EOF
            break;

        num_left -= num_read;
        ptr += num_read;
    }
    return n - num_left;
}

ssize_t BufferedReader::ReadLine(char* usrbuf, size_t maxlen, int flags/* = 0*/) {
    char ch;
    char* ptr = usrbuf;

    for (size_t i = 1; i != maxlen; ++i) {  // 最多读取maxlen-1次
        ssize_t num_read = this->Read(&ch, 1, flags);
        if (num_read == 1) {
            *ptr++ = ch;
            if (ch == '\n')  // 读取一行完毕
                break;
        } else if (num_read == 0) {  // EOF
            break;
        } else {  // 错误
            return -1;
        }
    }
    *ptr = '\0';
    return ptr - usrbuf;
}

std::string BufferedReader::GetUnreadBuffer() const {
    return std::string(buf_.data() + curpos_, leftcnt_);
}
