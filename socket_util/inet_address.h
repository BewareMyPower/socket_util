// inet_address.h
#pragma once

#include <arpa/inet.h>
#include <stdint.h>
#include <iostream>
#include <string>

namespace socket_util {

/**
 * 实用IPv4地址类，使用示例如下
 * // 1. 方便使用IPv4地址和端口号进行构造和赋值操作
 * InetAddress addr1;  // 0.0.0.0:0
 * addr1 = {"127.0.0.1", 1024};
 * cout << addr1 << endl;
 * InetAddress addr2{"192.168.100.1", 1008};
 * cout << addr2 << endl;
 * addr1 = addr2;
 * // 2. 方便传递参数给C API接口
 * auto sa_addr_ptr = addr1.getSockaddrPtr();
 * auto sa_addr_len = InetAddress::LENGTH;
 * accept(sockfd, sa_addr_ptr, &sa_addr_len);
 */
class InetAddress {
    friend std::ostream& operator << (std::ostream& os, const InetAddress& addr) {
        os << addr.toString();
        return os;
    }
public:
    constexpr static socklen_t LENGTH = sizeof(struct sockaddr_in);

    explicit InetAddress();  // 0.0.0.0:0
    InetAddress(const char* ip, uint16_t port);  // 若IP设置失败则置为"255.255.255.255"对应INADDR_NONE
    InetAddress(const struct sockaddr_in& addr);

    InetAddress(const InetAddress&) = default;
    InetAddress& operator=(const InetAddress&) = default;

    bool setIp(const char* new_ip) noexcept;  // 若IP不合法则返回false，且原有的IP不变
    void setPort(uint16_t new_port) noexcept { addr_.sin_port = htons(new_port); }

    std::string getIp() const noexcept;
    uint16_t getPort() const noexcept { return ntohs(addr_.sin_port); }

    std::string toString() const noexcept { return getIp() + ":" + std::to_string(getPort()); }

    const struct sockaddr* getSockaddrPtr() const noexcept;
    struct sockaddr* getSockaddrPtr() noexcept;

private:
    struct sockaddr_in addr_;
};

}  // END namespace socket_util
