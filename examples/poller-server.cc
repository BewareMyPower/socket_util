// g_poller-server.cc: 使用Poller监听多个socket的事件
// 所有套接字均为非阻塞，附带数据手动new和delete管理内存，考虑使用智能指针替代
#include <socket_util.h>
using namespace socket_util;

#define LOG(...) error::Println(__VA_ARGS__)

struct FdData {
    int fd;

    static FdData* cast(void* p) {
        error::ExitIf(!p, "FdData::cast nullptr");
        return reinterpret_cast<FdData*>(p);
    }
};

Poller g_poller;

inline void addAcceptFd(int listenfd) {
    int connfd = inet::accept(listenfd);
    error::ExitIf(connfd == -1, errno, "inet::accept");
    LOG("accept %d", connfd);

    inet::setnonblocking(connfd);

    if (!g_poller.add(connfd, EPOLLIN, new FdData{connfd}))
        error::Exit(errno, "Poller::add");
}

void recvCallBack(int fd) {
    char buf[4];  // 较小的缓冲区以观察多次recv
    auto to_uchar = [](char ch) { return static_cast<unsigned char>(ch); };
    while (true) {
        int num_recv = static_cast<int>(inet::recv(fd, buf, sizeof(buf)));
        if (num_recv > 0) {
            printf("recv %d bytes: [%02x", num_recv, to_uchar(buf[0]));
            for (int i = 1; i < num_recv; i++)
                printf(", %02x", to_uchar(buf[i]));
            printf("]\n");
        } else if (num_recv == 0) {
            printf("recv EOF\n");
            close(fd);
            break;
        } else {
            if (errno == EAGAIN)
                break;
            else
                error::ExitIf(errno, "inet::recv");
        }
    }
}

void handleEvent(int listenfd, const Poller::Event& event) {
    static size_t cnt = 0;
    auto data_ptr = FdData::cast(event.data.ptr);
    error::ExitIf(!data_ptr, "event data is null");

    int sockfd = data_ptr->fd;
    if (sockfd == listenfd) {
        addAcceptFd(listenfd);
    } else if (event.events & EPOLLIN) {
        LOG("event trigger %zu", cnt++);
        recvCallBack(sockfd);
    } else {
        LOG("something else happened");
    }
}

int main() {
    auto listenfd = inet::createTcpServer("localhost:8888");
    g_poller.add(listenfd, EPOLLIN, new FdData{listenfd});

    while (true) {
        auto events = g_poller.poll();
        error::ExitIf(events.empty(), "Poller::poll");

        for (auto const& event : events) {
            handleEvent(listenfd, event);
        }
    }

    close(listenfd);
    return 0;
}
