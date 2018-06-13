// inet_address.h
#pragma once

#include <arpa/inet.h>
#include <stdint.h>
#include <string>

namespace socket_util {

class InetAddress {
public:
    using SA = struct sockaddr;
    using SA_IN = struct sockaddr_in;

    explicit InetAddress(const char* ip, uint16_t port);  // ip不合法则报错并终止进程
    explicit InetAddress(const SA_IN& addr);

    std::string GetIp() const noexcept;  // ip不合法则报错并终止进程
    uint16_t GetPort() const noexcept { return ntohs(addr_.sin_port); }

    std::string ToString() const noexcept { return GetIp() + ":" + std::to_string(GetPort()); }

    const SA* GetSockaddrPtr() const noexcept;

    bool IsInvalid() const noexcept { return addr_.sin_port == 0; }

private:
    SA_IN addr_;
};

}
