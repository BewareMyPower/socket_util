// helper.h
#pragma once

#include "inet_socket.h"
#include "helpers/errors.hpp"
using namespace socket_util;

#include <fcntl.h>

namespace helper {

inline int acceptOneConnect(std::string&& address) {
    int sockfd = inet::createTcpServer(std::move(address), false);

    int connfd = inet::accept(sockfd);
    error::ExitIf(connfd == -1, errno, "accept");

    close(sockfd);
    return connfd;
}

inline int connectServer(std::string&& address) {
    int sockfd = inet::createTcpClient(std::move(address));
    error::ExitIf(sockfd == -1, errno, "inet::createTcpClient");

    return sockfd;
}

inline int open(const char* filename, int flags, mode_t mode = 0) {
    int fd = ::open(filename, flags, mode);
    error::ExitIf(fd == -1, errno, "open %s", filename);
    return fd;
}

}
