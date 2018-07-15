// simple-connect.cc: 简单地建立连接后断开
#include <iostream>

#include "inet_socket.h"
#include "util/errors.hpp"

using namespace std;
using namespace socket_util;
using namespace util;

int main() {
    cout << "client start connect..." << endl;
    int sockfd = inet::createTcpClient({"127.0.0.1", 8888});
    error::ExitIf(sockfd == -1, errno, "createTcpClient");

    sleep(2);
    cout << "client close connection" << endl;
    return 0;
}
