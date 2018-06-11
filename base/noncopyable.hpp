// noncopyable.hpp: noncopyable类的实现，参考boost::noncopyable类
// noncopyable类的派生类默认禁止了拷贝构造函数和拷贝赋值运算符
#pragma once

struct noncopyable {
    noncopyable() = default;

    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
};
