#pragma once

#include <arpa/inet.h>
#include <stdint.h>
#include <string>

namespace socket_util {

class InetAddress {
public:
    using SA = struct sockaddr;
    using SA_IN = struct sockaddr_in;

    explicit InetAddress(const char* ip, uint16_t port);
    explicit InetAddress(const SA_IN& addr);
    
    std::string GetIp() const;
    uint16_t GetPort() const;

    std::string ToString() const { return GetIp() + ":" + std::to_string(GetPort()); }

    const SA* GetSockaddrPtr() const;

private:
    SA_IN addr_;
};

}
