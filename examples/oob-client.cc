// oob-client.cc: 发送带外数据
#include "inet_socket.h"
using namespace socket_util;

int main(int argc, char* argv[]) {
    int sockfd = inet::createTcpClient("localhost:8888");

    inet::sendAll(sockfd, "123");
    inet::sendAll(sockfd, "abc", MSG_OOB);
    inet::sendAll(sockfd, "123");

    return 0;
}
