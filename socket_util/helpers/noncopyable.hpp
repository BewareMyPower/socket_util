// noncopyable.hpp: noncopyable类的实现，参考boost::noncopyable类
// noncopyable类的派生类默认禁止了拷贝构造函数和拷贝赋值运算符
#pragma once

namespace socket_util {

class noncopyable {
public:
    noncopyable() = default;
    ~noncopyable() = default;

    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
};

}  // END namespace socket_util
