// error_handler.h
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace error_handler {

// 打印格式化字符串format和参数args(同printf)，并打印换行符
template <typename ...Args> inline
void Print(const char* format, Args... args) {
    setvbuf(stderr, nullptr, _IOLBF, 0);  // stderr改成行缓冲
    fprintf(stderr, format, args...);
    fprintf(stderr, "\n");
    setvbuf(stderr, nullptr, _IONBF, 0);  // stderr恢复成不带缓冲
} 

template <typename ...Args> inline
void Print(const char* message) {
    fprintf(stderr, "%s\n", message);
}

// 在Print(bool, Args...)的基础上打印错误码error的信息，形式如下
// "[format和args构成的信息]: [error对应的信息]"
template <typename ...Args> inline
void Print(int error, const char* format, Args... args) {
    constexpr size_t ERROR_BUFLEN = 128;
    char error_buf[ERROR_BUFLEN];
    setvbuf(stderr, nullptr, _IOLBF, 0);  // stderr改成行缓冲
    fprintf(stderr, format, args...);
    fprintf(stderr, ": %s\n", strerror_r(error, error_buf, ERROR_BUFLEN));
    setvbuf(stderr, nullptr, _IONBF, 0);  // stderr恢复成不带缓冲
}

template <typename ...Args> inline
void Print(int error, const char* message) {
    constexpr size_t ERROR_BUFLEN = 128;
    char error_buf[ERROR_BUFLEN];
    fprintf(stderr, "%s: %s\n", message,
    strerror_r(error, error_buf, ERROR_BUFLEN));
}

template <typename ...Args> inline
void PrintIf(bool condition, Args... args) {
    if (condition)
        Print(args...);
}

template <typename ...Args> inline
void ExitIf(bool condition, Args... args) {
    if (condition) {
        Print(args...);
        exit(1);
    }
}

}
