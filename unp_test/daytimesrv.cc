// daytimesrv.cc: 时间获取服务端程序
#include "../include/ipv4_socket.hpp"
#include <time.h>

int main() {
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
}
