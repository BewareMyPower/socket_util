// daytimecli.cc: 时间获取客户程序
#include "../include/ipv4_socket.hpp"
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc != 2)
        err_quit("usage: %s [IPv4 address]", argv[0]);

    auto cli = CreateIpv4Socket();
    cli.Connect(argv[1], 8888);

    char buf[1024];
    ssize_t n;

    while ((n = cli.Recv(buf, sizeof(buf) - 1)) > 0) {
        buf[n] = '\0';
        if (fputs(buf, stdout) == EOF)
            err_sys("fputs");
    }
    if (n == -1)
        err_sys("recv error");

    return 0;
}
