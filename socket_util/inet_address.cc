// inet_address.cc
#include "inet_address.h"
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>

#include "helpers/errors.hpp"

namespace socket_util {

InetAddress::InetAddress() noexcept {
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
}

InetAddress::InetAddress(const struct sockaddr_in& addr) noexcept {
    memcpy(&addr_, &addr, sizeof(addr));
}

InetAddress::InetAddress(const char* ip, uint16_t port) noexcept {
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;

    if (!setIp(ip))
        addr_.sin_addr.s_addr = INADDR_NONE;
    addr_.sin_port = htons(port);
}

bool InetAddress::setIp(const char* new_ip) noexcept {
    error::ExitIf(!new_ip, "InetAddress::setIp(): ip is nullptr");
    struct in_addr old_addr = addr_.sin_addr;
    int ret = ::inet_pton(AF_INET, new_ip, &addr_.sin_addr);
    if (ret != 1) {
        addr_.sin_addr = old_addr;  // 还原
        return false;
    }
    return true;
}

InetAddress InetAddress::newInstance(const char* host, const char* serv) noexcept {
    if (!host || !*host)
        host = "127.0.0.1";

    struct addrinfo* info;
    auto errorcode = getaddrinfo(host, serv, nullptr, &info);
    if (errorcode == 0) {
        if (!info)
            error::Exit("getaddrinfo [host: %s, serv: %s] return NULL", host, serv);
        if (info->ai_family != AF_INET || info->ai_addrlen != sizeof(struct sockaddr_in))
            error::Exit("\"%s\" is not IPv4 address", host);

        InetAddress addr;
        memcpy(addr.getSockaddrPtr(), info->ai_addr, info->ai_addrlen);
        freeaddrinfo(info);
        return addr;
    } else {
        error::Exit("getaddrinfo \"%s:%s\" failed: %s", host, serv, gai_strerror(errorcode));
        return {"0.0.0.0", 0};
    }
}

InetAddress InetAddress::newInstance(std::string&& address) noexcept {
    size_t pos_colon = address.find(":");
    if (pos_colon == std::string::npos)
        error::Exit("%s doesn't contain ':'", address.data());
    if (pos_colon + 1 == address.size())
        error::Exit("%s doesn't contain servname or port", address.data());

    address[pos_colon] = '\0';
    return newInstance(address.data(), address.data() + pos_colon + 1);
}

const char* InetAddress::toCString() const noexcept {
    static char buf[INET_ADDRSTRLEN + 6];  // 冒号和端口号(0~65535)占最多6个字符

    const char* ret = ::inet_ntop(AF_INET, &addr_.sin_addr, buf, INET_ADDRSTRLEN);
    if (!ret) return "UNKNOWN IP";

    size_t curpos = strlen(buf);
    snprintf(buf + curpos, sizeof(buf) - curpos, ":%d", getPort());
    return buf;
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
