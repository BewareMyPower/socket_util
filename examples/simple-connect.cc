// simple-connect.cc: 简单地建立连接后断开
#include <iostream>

#include "inet_socket.h"
#include "util/errors.hpp"

using namespace std;
using namespace socket_util;
using namespace util;

int main() {
    int sockfd = inet::socket();
    error::ExitIf(sockfd == -1, errno, "createTcpServer");

    cout << "client start connect..." << endl;
    if (!inet::connect(sockfd, {"127.0.0.1", 8888}))
        error::Exit(errno, "connect");
    cout << "client connect success" << endl;

    sleep(2);
    cout << "client close connection" << endl;
    return 0;
}
