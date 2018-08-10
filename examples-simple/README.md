# examples-simple
简单功能的示例程序，比如仅仅用来展示`sendfile()`功能的sendfile.cc
该目录下的`.cc`文件均为独立编译的源文件，使用[socket\_util](../socket_util)库。

## 通用程序
helper.h:
    简化通用逻辑的头文件，由于局限性较强，这些函数未收录在socket\_util库内

## 单元测试
address-test.cc:
    InetAddress::newInstance()的测试程序

## 客户端
simple-client.cc:
    循环接收数据直至出错或EOF的客户程序
echo-client.cc:
    回射客户端，发送数据给服务器后接收同样大小的数据，直到EOF退出

## C-S结构示例
oob-server.cc:
    包含带外数据收发的服务器程序，接收客户发送的数据
oob-client.cc:
    包含带外数据收发的客户程序，发送数据给服务器

## 高级IO函数测试
cgi-server.cc:
    模拟CGI服务器，接收客户连接后用`dup2()`重定向标准输出到连接套接字，从而直接用`printf()`写入格式化数据到客户端
    对应客户端: simple-client.cc
writev.cc:
    `writev()`的测试，服务端向客户发送HTTP头和内容的拼接
    对应客户端: simple-client.cc
    示例发送文件: index.html
sendfile.cc:
    `sendfile()`的测试，服务端向客户用户态和内核态零拷贝发送文件
    对应客户端: simple-client.cc
    示例发送文件: index.html
splice.cc:
    `splice()`的测试，通过管道实现用户态和内核态零拷贝回射数据
    对应客户端: echo-client.cc
