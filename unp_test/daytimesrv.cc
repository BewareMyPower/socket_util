// daytimesrv.cc: 时间获取服务端程序
#include <stdio.h>
#include <time.h>
#include "../include/ipv4_socket.hpp"

int main() {
    auto listener = Ipv4Socket::Listener(8888);

    char buf[1024];
    while (true) {
        auto conn = listener.Accept();
        auto ticks = time(nullptr);
        snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
        
        conn.Send(buf, strlen(buf));
    }
}
