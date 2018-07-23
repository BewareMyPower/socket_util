// oob-client.cc: 发送带外数据
#include "inet_socket.h"
#include "util/errors.hpp"
#include <sys/select.h>

using namespace socket_util;
using namespace socket_util::util;

int main(int argc, char* argv[]) {
    int sockfd = inet::createTcpClient({"127.0.0.1", 8888});

    // 依次发送普通数据、带外数据、普通数据
    inet::sendAll(sockfd, "123");
    inet::sendAll(sockfd, "abc", MSG_OOB);
    inet::sendAll(sockfd, "123");

    return 0;
}
