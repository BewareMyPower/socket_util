// tcpechocli.cc: TCP回射客户程序
#include <stdio.h>
#include <string.h>
#include <vector>
#include "../include/ipv4_socket.hpp"
#include "../include/buffered_reader.hpp"
#include "../include/error_functions.hpp"

static void str_cli(FILE* fp, Ipv4Socket& conn);  // 客户建立连接后的处理

int main(int argc, char* argv[]) {
    if (argc != 2)
        err_quit("usage: %s [IPv4 address]", argv[0]);

    // 与并发服务器建立5个连接
    Ipv4Socket clients[5];
    for (auto& cli : clients)
        cli.Connect(argv[1], 8888);
    str_cli(stdin, clients[0]);
    return 0;
}

static void str_cli(FILE* fp, Ipv4Socket& conn) {
    // 从fp中读取一行文本并将其发送给服务器，然后从服务器中读入回射行
    char sendline[1024], recvline[1024];
    BufferedReader reader(conn.GetFd(), 1024);

    while (fgets(sendline, sizeof(sendline), fp) != nullptr) {
        if (conn.SendNBytes(sendline, strlen(sendline)) == -1)
            err_sys("send");

        if (reader.ReadLine(recvline, sizeof(recvline)) == 0)
            err_quit("str_cli: server terminated prematurely");

        if (fputs(recvline, stdout) == EOF)
            err_sys("fputs");
    }
    if (ferror(fp))
        err_sys("fgets");
}
