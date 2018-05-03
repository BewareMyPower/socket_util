# socket_util
对linux的socket库函数的C++封装

# [include](include)目录为所需头文件，目前类的成员函数均为inline函数，因此仅需包含头文件即可使用，需要用-std=c++11选项进行编译。
## [error_functions.h](include/error_functions.h)
和APUE/UNP系列相同的错误处理函数，内部不使用静态字符数组，而是用std::string保存错误信息
## [socket.hpp](include/socket.hpp)
- Socket类的实现
- 该类无法直接构造，必须public继承后并将构造函数设为public权限
- Socket类维护了一个套接字，封装了listen/send/recv/close/等仅和套接字相关的函数，而像bind/accept等和套接字地址相关的函数则由派生类定义
- 为了与<sys/socket.h>区分开因此使用hpp作为后缀
- 不使用虚基类原因在于，派生类无需重写仅和套接字相关的函数
## [ipv4_socket.h](include/ipv4_socket.h)
继承自Socket类的Ipv4Socket类的实现，包含了[ipv4_addr.h](include/ipv4_addr.h)（IPv4地址的包装）

# 命名规范
类和函数的命名均为大小写混合，对应文件的命名均为下划线+小写，参考谷歌C++命名约定。

# 额外说明
头文件使用了`#pragma once`宏来防止头文件被多重包含，注意这个特性并不在C++标准中，只不过受到了大多数编译器(比如g++)的支持，严格遵守标准的代码应该使用`#ifndef`和`#define`来实现。
