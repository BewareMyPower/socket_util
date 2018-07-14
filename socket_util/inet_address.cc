// inet_address.cc
#include "inet_address.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace socket_util {

InetAddress::InetAddress() {
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
}

InetAddress::InetAddress(const char* ip, uint16_t port) {
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;

    if (!setIp(ip))
        addr_.sin_addr.s_addr = INADDR_NONE;
    addr_.sin_port = htons(port);
}

InetAddress::InetAddress(const struct sockaddr_in& addr) {
    memcpy(&addr_, &addr, sizeof(addr));
}

bool InetAddress::setIp(const char* new_ip) noexcept {
    struct in_addr old_addr = addr_.sin_addr;
    int ret = ::inet_pton(AF_INET, new_ip, &addr_.sin_addr);
    if (ret != 1) {
        addr_.sin_addr = old_addr;  // 还原
        return false;
    }
    return true;
}

std::string InetAddress::getIp() const noexcept {
    char buf[INET_ADDRSTRLEN];
    const char* ret = ::inet_ntop(AF_INET, &addr_.sin_addr, buf, INET_ADDRSTRLEN);
    return ret ? ret : "UNKNOWN IP";
}

const struct sockaddr* InetAddress::getSockaddrPtr() const noexcept {
    return static_cast<const struct sockaddr*>(
               const_cast<InetAddress*>(this)->getSockaddrPtr()
           );
}

struct sockaddr* InetAddress::getSockaddrPtr() noexcept {
    return reinterpret_cast<struct sockaddr*>(&addr_);
}

}  // END namespace socket_util
