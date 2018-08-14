// poller.cc
#include "poller.h"
#include "helpers/errors.hpp"
#include <assert.h>

namespace socket_util {

Poller::Poller() noexcept {
    epfd_ = epoll_create(1);
    error::ExitIf(epfd_ == -1, errno, "epoll_create");
}

Poller::~Poller() {
    close(epfd_);
}

bool Poller::add(int fd, uint32_t events, void* opaque) noexcept {
    struct epoll_event ev;
    ev.events = events;
    ev.data.ptr = opaque;
    if (!ev.data.ptr)
        ev.data.fd = fd;

    int ret = epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev);
    if (ret == -1) {
        return false;
    } else {
        num_events_++;
        return true;
    }
}

bool Poller::remove(int fd) noexcept {
    // NOTE: Linux 2.6.9后最后一个参数可以直接传入NULL，参见epoll_ctl(2)的BUGS
    struct epoll_event ev;
    int ret = epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, &ev);
    if (ret == -1) {
        return false;
    } else {
        assert(num_events_ > 0);
        num_events_--;
        return true;
    }
}

bool Poller::change(int fd, uint32_t events, void* opaque) const noexcept {
    struct epoll_event ev;
    ev.events = events;
    ev.data.ptr = opaque;

    return epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &ev) != -1;
}

Poller::EventList Poller::poll(int timeout_ms) const noexcept {
    assert(timeout_ms >= 0 || timeout_ms == -1);
    int old_errno = errno;
    EventList events(num_events_);

    while (true) {
        int num_wait = epoll_wait(epfd_, events.data(), events.size(), timeout_ms);
        if (num_wait < 0) {
            if (errno == EINTR) {  // 重启被信号中断的epoll_wait
                errno = old_errno;
                continue;
            } else {
                events.clear();
            }
        } else if (num_wait == 0) {
            events.clear();
        } else {
            events.resize(static_cast<size_t>(num_wait));
        }
        break;
    }
    return events;
}

}  // END namespace socket_util
