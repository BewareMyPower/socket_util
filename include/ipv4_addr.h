#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>

#include <string>
#include <stdexcept>

class Ipv4Addr {
public:
    using SA = struct sockaddr;
    using SA_IN = struct sockaddr_in;

    // ip:   点分十进制表示的IPv4地址
    // port: 本机字节序的端口号
    explicit Ipv4Addr(const std::string& ip, uint16_t port);

    explicit Ipv4Addr(const SA_IN& addr);

    std::string GetIp() const;  // 返回点分十进制表示的IPv4地址
    uint16_t GetPort() const;  // 返回本机字节序的端口号
    std::string ToString() const;  // 转换成用冒号分隔开IP和端口的表达形式，比如"0.0.0.0:22"

    const SA* GetPtrToSA() const { return reinterpret_cast<const SA*>(&addr_); }
private:
    SA_IN addr_;  // 底层表示的地址
};

Ipv4Addr::Ipv4Addr(const std::string& ip, uint16_t port) {
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);  // 转换成网络字节序

    int err = inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr);
    if (err == 0) {
        fprintf(stderr, "\"%s\"不是有效的IPv4表达格式\n", ip.c_str());
        exit(1);
    } else if (err == -1) {
        fprintf(stderr, "inet_pton %s error: %s\n", ip.c_str(), strerror(errno));
        exit(1);
    }
}

Ipv4Addr::Ipv4Addr(const SA_IN& addr) {
    memcpy(&addr_, &addr, sizeof(SA_IN));
}

inline std::string Ipv4Addr::GetIp() const {
    char buffer[INET_ADDRSTRLEN];
    const char* s = inet_ntop(AF_INET, &addr_.sin_addr, buffer, sizeof(buffer));
    if (!s)
        return "[invalid ip]";
    return s;  // convert to std::string
}

inline uint16_t Ipv4Addr::GetPort() const {
    return ntohs(addr_.sin_port);
}

inline std::string Ipv4Addr::ToString() const {
    return GetIp() + ":" + std::to_string(GetPort());
}
