// address_test.cc
#include "inet_socket.h"
#include "helpers/errors.hpp"
using namespace socket_util;

#include <unistd.h>
#include <sys/wait.h>

#include <iostream>
using namespace std;

inline pid_t Fork() {
    auto pid = fork();
    error::ExitIf(pid == -1, errno, "fork");
    return pid;
}

int main() {
    cout << InetAddress::newInstance("127.0.0.1:443") << endl;
    cout << InetAddress::newInstance("127.0.0.1:https") << endl;
    cout << InetAddress::newInstance("localhost:443") << endl;
    cout << InetAddress::newInstance("localhost:https") << endl;
    cout << InetAddress::newInstance(":443") << endl;
    cout << InetAddress::newInstance(":443") << endl;
    cout << InetAddress::newInstance(nullptr, "443") << endl;
    cout << InetAddress::newInstance(nullptr, "https") << endl;

    if (Fork() == 0) {
        cout << InetAddress::newInstance("localhost") << endl;
        _exit(0);
    } else {
        wait(nullptr);
    }

    if (Fork() == 0) {
        cout << InetAddress::newInstance(":9000000000000000") << endl;
        _exit(0);
    } else {
        wait(nullptr);
    }

    if (Fork() == 0) {
        InetAddress addr{nullptr, 443};
        cout << addr << endl;
        _exit(0);
    } else {
        wait(nullptr);
    }

    return 0;
}
