// fd_guard.hpp: RAII包装文件描述符(fd, file descriptor)，保证离开作用域时调用close()
// FdGuard的使用场景是保证离开作用域时，fd能够自动关闭，示例如下
// bool do_sth() {
//     int fd = open_some_resource();  // 打开文件资源，可以是open、socket等
//     FdGuard guard{fd};
//     if (do_sth_1(fd)) return false;
//     if (do_sth_2(fd)) return false;
//     // ...
//     return true;  // 无论返回值如何，fd都会被close
// }
//
// int create_my_fd() {
//     int fd = open_some_resource();
//     FdGuard guard{fd};
//     // 检测失败时返回-1，guard析构函数调用，关闭创建的fd
//     if (check1(fd)) return -1;
//     if (check2(fd)) return -1;
//     // ...
//     // 通过所有检测，调用分离方法防止guard析构函数关闭fd
//     guard.detach();
//     return fd;
// }
#pragma once

#include "noncopyable.hpp"

extern "C" int close(int);

namespace socket_util {

namespace util {

class FdGuard : noncopyable {
public:
    explicit FdGuard(int fd) : fd_(fd) { }

    ~FdGuard() {
        if (fd_ != -1)
            close(fd_);
    }

    void detach() noexcept { fd_ = -1; }

    int get() const noexcept { return fd_; }

private:
    int fd_;
};

}  // END namespace socket_util::util

}  // END namespace socket_util
