#pragma once

#include <arpa/inet.h>
#include <string>
#include "error_functions.hpp"

class Ipv4Addr {
public:
    using SA = struct sockaddr;
    using SA_IN = struct sockaddr_in;

    // ip:   点分十进制表示的IPv4地址
    // port: 本机字节序的端口号
    explicit Ipv4Addr(const char* ip, uint16_t port);
    explicit Ipv4Addr(const std::string& ip, uint16_t port) : Ipv4Addr(ip.c_str(), port) {}
    explicit Ipv4Addr(const SA_IN& addr);

    std::string GetIp() const;  // 返回点分十进制表示的IPv4地址
    uint16_t GetPort() const;  // 返回本机字节序的端口号
    std::string ToString() const;  // 转换成用冒号分隔开IP和端口的表达形式，比如"0.0.0.0:22"

    const SA* GetPtrToSA() const { return reinterpret_cast<const SA*>(&addr_); }
private:
    SA_IN addr_;  // 底层表示的地址
};

Ipv4Addr::Ipv4Addr(const char* ip, uint16_t port) {
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);  // 转换成网络字节序

    int err = inet_pton(AF_INET, ip, &addr_.sin_addr);
    if (err == 0)
        err_quit("\"%s\"不是有效的IPv4表达格式", ip);
    else if (err == -1)
        err_sys("inet_pton %s error", ip);
}

Ipv4Addr::Ipv4Addr(const SA_IN& addr) {
    memcpy(&addr_, &addr, sizeof(addr));
}

inline std::string Ipv4Addr::GetIp() const {
    char buffer[INET_ADDRSTRLEN];
    const char* s = inet_ntop(AF_INET, &addr_.sin_addr, buffer, sizeof(buffer));
    return (s != nullptr) ? s : "*invalid ip*";
}

inline uint16_t Ipv4Addr::GetPort() const {
    return ntohs(addr_.sin_port);
}

inline std::string Ipv4Addr::ToString() const {
    return GetIp() + ":" + std::to_string(GetPort());
}
