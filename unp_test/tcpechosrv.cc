// tcpechosrv.cc: TCP回射服务器程序
#include <stdio.h>
#include <unistd.h>
#include "../include/ipv4_socket.hpp"
#include "../include/sig_util.h"

static void str_echo(Ipv4Socket& conn);  // 服务端建立连接后的处理

int main() {
    auto listener = CreateIpv4Socket();
    listener.SetSockOpt(SOL_SOCKET, SO_REUSEADDR, 1);
    listener.Bind("0.0.0.0", 8888);
    listener.Listen();
    Signal(SIGCHLD, sig_child);

    while (true) {
        auto res = listener.AcceptWithAddr();
        auto& conn = res.first;
        auto& cliaddr = res.second;
        printf("accept client: %s\n", cliaddr.ToString().c_str());

        pid_t childpid = fork();
        if (childpid == -1) {
            err_sys("fork");
        } else if (childpid == 0) {  // 子进程
            listener.Close();
            str_echo(conn);
            exit(0);  // conn被析构，自动关闭
        }

        conn.Close();
    }

    return 0;
}

static void str_echo(Ipv4Socket& conn) {
    // 从套接字中读取数据，把其中内容回射给客户
    char buf[1024];

    while (true) {
        ssize_t n = conn.Recv(buf, sizeof(buf));
        if (n > 0) {
            conn.Send(buf, n);
        } else if (n == 0) {  // EOF
            printf("[pid: %ld] done!\n", (long)getpid());
            break;
        } else {  // n == -1
            if (errno != EINTR)
                err_sys("str_echo: recv error");
        }
    }
}
