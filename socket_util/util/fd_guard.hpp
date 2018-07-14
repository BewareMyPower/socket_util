// fd_guard.hpp: RAII包装文件描述符(fd, file descriptor)，保证离开作用域时调用close()
#pragma once

#include "noncopyable.hpp"

extern "C" int close(int);

namespace util {

class FdGuard : noncopyable {
public:
    explicit FdGuard(int& fd) : fd_(fd) { fd = -1; }

    ~FdGuard() {
        if (fd_ != -1)
            close(fd_);
    }

    void detach() noexcept { fd_ = -1; }

    int get() const noexcept { return fd_; }

private:
    int fd_;
};

}  // END namespace util
