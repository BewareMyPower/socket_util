// multi-client.cc: 多线程，每个线程连接到服务端发送一段标识自己身份数据
#include <socket_util.h>
using namespace socket_util;

#include <thread>
#include <chrono>

thread_local char buf[] = " : hello";

void sendData(char id, int timeout_ms) {
    int old_errno = errno;
    buf[0] = id;

    int sockfd = inet::createTcpClient("localhost:8888");
    if (sockfd == -1) {
        error::Println(errno, "inet::createTcpClient");
        errno = old_errno;
        return;
    }
    util::FdGuard guard{sockfd};

    std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));
    if (!inet::send(sockfd, buf, sizeof(buf) / sizeof(char))) {
        error::Println(errno, "inet::sendCString");
        errno = old_errno;
        return;
    }
}

int main(int argc, char* argv[]) {
    error::ExitIf(argc != 2, "Usage: %s thread-num", argv[0]);

    char* endptr = nullptr;
    int num_threads = strtol(argv[1], &endptr, 10);
    if (num_threads < 0) {
        error::Exit("thread-num must >0");
    } else if (*endptr != '\0') {
        error::Exit("\"%s\" is not a number", argv[1]);
    }

    std::vector<std::thread> threads(num_threads);
    for (int i = 0; i < num_threads; ++i)
        threads[i] = std::thread{sendData, static_cast<char>(i), 1300 - i};
    for (auto& th : threads)
        th.join();
    return 0;
}
