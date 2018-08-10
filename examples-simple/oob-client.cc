// oob-client.cc: 发送带外数据
#include "helper.h"

int main(int argc, char* argv[]) {
    int sockfd = helper::connectServer("localhost:8888");

    inet::sendCString(sockfd, "123");
    inet::sendCString(sockfd, "abc", MSG_OOB);
    inet::sendCString(sockfd, "123");

    return 0;
}
