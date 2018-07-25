// errors.hpp: 定义了一组错误处理函数于命名空间error内，将格式化字符串打印到标准错误流上
// 内部缓冲区大小是硬编码，因此无法自定义大小，过长的错误字符串会被截断，因此对于较长的出错信息，
// 比如提示较多命令行参数的用法的"Usage: ..."字符串不应该使用这类函数。
// 常见的应用场景是检查函数的返回值，参考下列代码
// int main(int argc, char* argv[]) {
//     using namespace util;
//     error::ExitIf(argc != 2, "Usage: %s filename");
//
//     int fd = open(argv[1], O_RDONLY);
//     util::error::ExitIf(fd == -1, errno, "open %s", argv[1]);
//     return 0;
// }
// 注意ExitIf不要滥用，如果参数1的条件表达式可能改变参数2的值，那么应该分开写，比如
// error::ExitIf(open("myfile", O_RDONLY) == -1, errno, "open myfile");
// 由于参数1是一个待执行的语句，传参时的errno在执行参数1的语句后可能发生改变，而参数2是值传递，因此不会更新为改变后的值。
// 正确的使用方式是将返回错误码的语句单独作为一行
// if (open("myfile", O_RDONLY) == -1)
//     error::Exit(errno, "open myfile");
#pragma once

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <string>

namespace socket_util {

namespace error {

const char* failed_str = "???";
const size_t BUFFER_SIZE = 1024;  // 内部缓冲区大小

inline void Print(const char* message) {
    write(STDERR_FILENO, message, strlen(message));
}

inline void Print(const std::string& message) {
    write(STDERR_FILENO, message.data(), message.size());
}

template <typename ...Args>
inline void Print(const char* format, Args... args) {
    fprintf(stderr, format, args...);
}

inline void Println(const char* message) {
    char buffer[BUFFER_SIZE];
    int num_print = snprintf(buffer, sizeof(buffer), "%s\n", message);
    write(STDERR_FILENO, buffer, num_print);
}

inline void Println(const std::string& message) {
    Println(message.data());
}

template <typename ...Args>
inline void Println(const char* format, Args... args) {
    char buffer[BUFFER_SIZE];
    int num_print = snprintf(buffer, sizeof(buffer) - 1, format, args...);
    if (num_print < 0) {
        strcpy(buffer, failed_str);
        num_print = strlen(failed_str);
    }
    buffer[num_print++] = '\n';
    write(STDERR_FILENO, buffer, num_print);
}

template <typename ...Args>
inline void Println(int errnum, const char* format, Args... args) {
    char error_buffer[128];
    char buffer[BUFFER_SIZE];

    int num_print = snprintf(buffer, sizeof(buffer), format, args...);
    if (num_print < 0) {
        strcpy(buffer, failed_str);
        num_print = strlen(failed_str);
    }

    if (static_cast<size_t>(num_print) + 1 < sizeof(buffer)) {
        num_print += snprintf(buffer + num_print, sizeof(buffer) - num_print,
                              ": %s\n",
                              strerror_r(errnum, error_buffer, sizeof(error_buffer)));
    }

    write(STDERR_FILENO, buffer, num_print);
}

template <typename ...Args>
inline void Println(int errnum, const char* message) {
    Println(errnum, "%s", message);
}

template <typename ...Args>
inline void Println(int errnum, const std::string& message) {
    Println(errnum, "%s", message.data());
}

template <typename ...Args>
inline void Exit(Args... args) {
    Println(std::forward<Args>(args)...);
    exit(1);
}

template <typename ...Args>
inline void ExitIf(bool condition, Args... args) {
    if (condition)
        Exit(std::forward<Args>(args)...);
}

}  // END namespace socket_util::error

}  // END namespace socket_util
