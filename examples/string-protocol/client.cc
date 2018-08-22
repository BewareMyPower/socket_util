// client.cc
#include <socket_util.h>
using namespace socket_util;

#include "protocol.h"

int main(int argc, char* argv[]) {
    int clifd = inet::createTcpClient("localhost:8888");
    error::ExitIf(clifd == -1, errno, "inet::createTcpClient");

    protocol::String buf;
    while (buf.getline(stdin)) {
        buf.removeNewline();
        printf("[client] size: %u\n", buf.getLength());
        printf("[client] data: %s\n", buf.getBuffer().data());
        if (!buf.sendTo(clifd, MSG_NOSIGNAL))
            error::Exit(errno, "protocol::String::send");
    }

    close(clifd);
    return 0;
}
