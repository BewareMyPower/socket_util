#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include <string>
#include <vector>

/**
 * 功能: 用类似printf的方式构建C++标准字符串
 * 参数: 同printf(3)的输入参数
 * 返回: 调用printf(format, args...)时打印到屏幕上的字符串
 */
template <typename ...Args>
inline std::string format_string(const char* format, Args... args) {
    constexpr size_t oldlen = BUFSIZ;
    char buffer[oldlen];  // 默认栈上的缓冲区

    size_t newlen = snprintf(&buffer[0], oldlen, format, args...);
    newlen++;  // 算上终止符'\0'

    if (newlen > oldlen) {  // 默认缓冲区不够大，从堆上分配
        std::vector<char> newbuffer(newlen);
        snprintf(newbuffer.data(), newlen, format, args...);
        return std::string(newbuffer.data());
    }

    return buffer;
}

/**
 * 非致命性错误，系统调用无关
 * 打印消息后返回
 */
template <typename ...Args>
inline void err_msg(const char* fmt, Args... args) {
    auto msg = format_string(fmt, args...);
    fprintf(stderr, "%s\n", msg.c_str());
}

inline void err_msg(const char* msg) {
    fprintf(stderr, "%s\n", msg);
}

/**
 * 非致命性错误，系统调用相关
 * 打印消息后返回
 */
template <typename ...Args>
inline void err_ret(const char* fmt, Args... args) {
    std::string msg = format_string(fmt, args...);
    msg.append(": ");
    msg.append(strerror(errno));
    fprintf(stderr, "%s\n", msg.c_str());
}

inline void err_ret(const char* msg) {
    std::string msg_with_errno = msg;
    msg_with_errno.append(": ");
    msg_with_errno.append(strerror(errno));
    fprintf(stderr, "%s\n", msg_with_errno.c_str());
}

/**
 * 非致命性错误，系统调用无关
 * 显式传递错误码
 * 打印消息后返回
 */
template <typename ...Args>
inline void err_cont(int error, const char* fmt, Args... args) {
    auto msg = format_string(fmt, args...);
    msg.append(": ");
    msg.append(strerror(error));
    fprintf(stderr, "%s\n", msg.c_str());
}

/**
 * 致命性错误，系统调用无关
 * 打印消息后终止进程
 */
template <typename ...Args>
inline void err_quit(const char* fmt, Args... args) {
    err_msg(fmt, std::forward<Args>(args)...);
    exit(1);
}

/**
 * 致命性错误，系统调用相关
 * 打印消息后终止进程
 */
template <typename ...Args>
inline void err_sys(const char* fmt, Args... args) {
    err_ret(fmt, std::forward<Args>(args)...);
    exit(1);
}

/**
 * 非致命性错误，系统调用无关
 * 显式传递错误码
 * 打印消息后终止进程
 */
template <typename ...Args>
inline void err_exit(int error, const char* fmt, Args... args) {
    err_cont(error, fmt, std::forward<Args>(args)...);
    exit(1);
}

/**
 * 致命性错误，系统调用相关
 * 打印消息，转储核心后后终止进程
 */
template <typename ...Args>
inline void err_dump(const char* fmt, Args... args) {
    err_ret(fmt, std::forward<Args>(args)...);
    abort();
}
