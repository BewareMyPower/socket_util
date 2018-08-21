// io_util.h
#pragma once

#include <sys/types.h>
#include <sys/uio.h>
#include <functional>

namespace socket_util {

namespace io_util {

using ReadFunc = std::function<ssize_t(int, void*, size_t)>;
using WriteFunc = std::function<ssize_t(int, const void*, size_t)>;

/**
 * 功能: 写入整个缓冲区
 * 参数:
 *   writeFunc 写入函数
 *   fd        待写入的文件描述符
 *   buf       指向缓冲区首地址
 *   n         写入的字节数
 * 返回值:
 *   若全部写入成功则返回true，否则返回false，errno被设置
 */
bool writeAll(WriteFunc writeFunc, int fd, const void* buf, size_t n) noexcept;

/**
 * 功能: 读取n个字节到缓冲区
 * 参数:
 *   readFunc  读取函数
 *   fd        待读取的文件描述符
 *   buf       指向缓冲区首地址
 *   n         读取的字节数
 * 返回值:
 *   读取的字节数量，若读取失败则返回-1，errno被设置
 * 说明:
 *   正常情况下返回值为n，若中途读取到EOF则返回值会小于n。
 */
ssize_t readNBytes(ReadFunc readFunc, int fd, void* buf, size_t n) noexcept;

}  // END namespace io_util

}  // END namespace socket_util
