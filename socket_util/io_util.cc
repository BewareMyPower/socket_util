// io_util.cc
#include "io_util.h"
#include <errno.h>

namespace socket_util {

namespace io_util {

bool writeAll(WriteFunc writeFunc, int fd, const void* buf, size_t n) noexcept {
    auto curpos = static_cast<const char*>(buf);
    size_t num_left = n;

    while (num_left > 0) {
        ssize_t num_send = writeFunc(fd, curpos, num_left);
        if (num_send == -1) {
            if (errno == EINTR) {
                num_send = 0;  // 重启被中断的writeFunc
            } else {
                return false;
            }
        }

        num_left -= num_send;
        curpos += num_send;
    }
    return true;
}

bool readNBytes(ReadFunc readFunc, int fd, void* buf, size_t n) noexcept {
    ssize_t num_read = readAsMuchAsPossible(readFunc, fd, buf, n);
    return num_read == static_cast<ssize_t>(n);
}

ssize_t readAsMuchAsPossible(ReadFunc readFunc, int fd, void* buf, size_t bufsize) noexcept {
    auto num_left = static_cast<ssize_t>(bufsize);
    auto ptr = static_cast<char*>(buf);

    while (num_left > 0) {
        ssize_t num_read = readFunc(fd, ptr, num_left);
        if (num_read == -1) {
            if (errno == EAGAIN) {
                break;  // 非阻塞模式下无多余数据可读
            } else if (errno == EINTR) {
                continue;  // 重启被中断的readFunc
            } else {
                return -1;
            }
        } else if (num_read == 0) {
            break;  // 读到EOF
        }

        num_left -= num_read;
        ptr += num_read;
    }

    return static_cast<ssize_t>(bufsize) - num_left;
}

}  // END namespace io_util

}  // END namespace socket_util
