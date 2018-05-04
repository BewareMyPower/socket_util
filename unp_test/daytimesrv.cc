#include "../include/ipv4_socket.h"
#include <time.h>

int main(int argc, char* argv[]) {
    if (argc != 2)
        err_quit("usage: %s [IPv4 address]", argv[0]);

    auto listener = CreateIpv4Socket();
    listener.Bind("0.0.0.0", 8888);
    listener.Listen();

    char buf[1024];
    while (true) {
        auto conn = listener.Accept();
        auto ticks = time(nullptr);
        snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
        
        conn.Send(buf, strlen(buf));
    }

    return 0;
}
