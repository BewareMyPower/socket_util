// socket_base.cc
#include "socket_base.h"
#include <errno.h>

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

}
