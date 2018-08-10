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
 *
 * 支持用工厂方法来简单地构造
 * auto address = InetAddress::newInstance("localhost:https")
 */
class InetAddress {
    friend std::ostream& operator << (std::ostream& os, const InetAddress& addr) {
        os << addr.toString();
        return os;
    }
public:
    constexpr static socklen_t LENGTH = sizeof(struct sockaddr_in);

    explicit InetAddress() noexcept;  // 0.0.0.0:0
    InetAddress(const struct sockaddr_in& addr) noexcept;

    // 若IP为NULL则报错并退出程序
    // 若IP不合法则设置IP为INADDR_NONE(即255.255.255.255)
    InetAddress(const char* ip, uint16_t port) noexcept;

    /**
     * 功能: 通过主机名和服务名来构造InetAddress对象
     * 参数:
     *   host 主机名或者点分十进制的IP地址，若为NULL或空字符串则为"127.0.0.1"
     *   serv 服务名或者端口号
     * 返回值:
     *   若构造成功则返回对应InetAddress对象，否则打印错误到标准错误流并退出程序
     */
    static InetAddress newInstance(const char* host, const char* serv) noexcept;

    /**
     * 功能: 通过':'分隔的主机名和服务名来构造InetAddress对象
     * 参数:
     *   address ':'分隔后的2个字符串同newInstance重载的host和serv，参考下列示例
     *     127.0.0.1:443
     *     127.0.0.1:https
     *     localhost:443
     *     localhost:https
     *     :443
     *     :https
     * 返回值:
     *   若构造成功则返回对应InetAddress对象，否则打印错误到标准错误流并退出程序
     */
    static InetAddress newInstance(std::string&& address) noexcept;

    InetAddress(const InetAddress&) = default;
    InetAddress& operator=(const InetAddress&) = default;

    /**
     * 功能: 返回"[IP]:[Port]"格式的字符串标识存储的地址，此方法不可重入
     * 返回值:
     *   若转换成功，则返回值可以作为newInstance()的输入参数；
     *   若IP解析失败，则返回"UNKNOWN IP"；
     */
    const char* toCString() const noexcept;

    // 若IP为NULL则报错并退出程序，若设置失败则返回false，IP不变
    bool setIp(const char* new_ip) noexcept;
    void setPort(uint16_t new_port) noexcept { addr_.sin_port = htons(new_port); }

    // 若IP解析失败，则返回"UNKNOWN IP"
    std::string getIp() const noexcept;
    uint16_t getPort() const noexcept { return ntohs(addr_.sin_port); }

    std::string toString() const noexcept { return getIp() + ":" + std::to_string(getPort()); }

    const struct sockaddr* getSockaddrPtr() const noexcept;
    struct sockaddr* getSockaddrPtr() noexcept;

private:
    struct sockaddr_in addr_;
};

}  // END namespace socket_util
