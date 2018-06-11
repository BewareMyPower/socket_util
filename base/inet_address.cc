#include "inet_address.h"
#include <errno.h>
#include <string.h>
#include "error_handler.hpp"

namespace socket_util {

using SA = InetAddress::SA;
using SA_IN = InetAddress::SA_IN;

InetAddress::InetAddress(const char* ip, uint16_t port) {
    bzero(&addr_, sizeof(addr_));

    int ret = ::inet_pton(AF_INET, ip, &addr_.sin_addr);
    error_handler::ExitIf(0 == ret, "\"%s\"不是有效的IPv4地址", ip);
    error_handler::ExitIf(-1 == ret, errno, "inet_pton (%s)", ip);

    addr_.sin_port = htons(port);
}

InetAddress::InetAddress(const SA_IN& addr) {
    memcpy(&addr_, &addr, sizeof(addr));
}

std::string InetAddress::GetIp() const {
    char buf[INET_ADDRSTRLEN];
    const char* ret = ::inet_ntop(AF_INET, &addr_.sin_addr, buf, INET_ADDRSTRLEN);
    error_handler::ExitIf(!ret, errno, "inet_ntop");
    return ret;
}

uint16_t InetAddress::GetPort() const {
    return ntohs(addr_.sin_port);
}

}
