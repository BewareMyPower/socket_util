// simple-connect.cc: 简单地建立连接后断开
#include <iostream>

#include "inet_socket.h"
#include "util/errors.hpp"

using namespace std;
using namespace socket_util;
using namespace socket_util::util;

int main() {
    int sockfd = inet::createTcpServer({"0.0.0.0", 8888}, false);
    error::ExitIf(sockfd == -1, errno, "createTcpServer");
    cout << "server listen at " << inet::getsockname(sockfd) << endl;

    int connfd = inet::accept(sockfd);
    error::ExitIf(connfd == -1, errno, "accept");
    cout << "accept client: " << inet::getpeername(connfd) << endl;

    sleep(1);
    cout << "server close connection" << endl;
    return 0;
}
/**
# ./simple-accept &
[1] 2929
# server listen at 0.0.0.0:8888

# ./simple-connect
client start connect...
accept client: 127.0.0.1:54510
client connect success
server close connection
client close connection
[1]+  Done                    ./simple-accept
 */
