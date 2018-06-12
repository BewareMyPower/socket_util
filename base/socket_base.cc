// socket_base.cc
#include "socket_base.h"
#include <errno.h>
#include <fcntl.h>

namespace socket_util {

bool SocketBase::SendNBytes(
        const char* buf, size_t n, int flags) const noexcept {
    size_t num_left = n;

    while (num_left > 0) {
        ssize_t num_send = Send(buf, num_left, flags);
        if (num_send == -1) {
            if (errno == EINTR)  // 被信号中断则重新send
                num_send = 0;
            else
                return false;
        }

        num_left -= num_send;
        buf += num_send;
    }

    return n;
}

bool SocketBase::SetNonblocking(bool doit) const noexcept{
    int flags = ::fcntl(fd_, F_GETFL, 0);
    if (-1 == flags)
        return false;

    if (doit)
        flags |= O_NONBLOCK;
    else
        flags &= ~O_NONBLOCK;

    return ::fcntl(fd_, F_SETFL, flags);
}

}
