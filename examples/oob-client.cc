// oob-client.cc: 发送带外数据
#include "inet_socket.h"
#include "util/errors.hpp"
#include <sys/select.h>

using namespace socket_util;
using namespace socket_util::util;

int main(int argc, char* argv[]) {
    error::ExitIf(argc <= 2, "Usage: %s ip port", basename(argv[0]));

    const char* ip = argv[1];
    uint16_t port = atoi(argv[2]);
    int sockfd = inet::createTcpClient({ip, port});
    error::ExitIf(sockfd == -1, errno, "createTcpClient");

    // 依次发送普通数据、带外数据、普通数据
    inet::send(sockfd, "123");
    inet::send(sockfd, "abc", MSG_OOB);
    inet::send(sockfd, "123");

    return 0;
}
