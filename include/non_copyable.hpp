// non_copyable.hpp: NonCopyable类的实现，参考boost::noncopyable类
// NonCopyable类的派生类默认禁止了拷贝构造函数和拷贝赋值运算符
// 同时派生类拥有默认合成的构造函数和析构函数
#pragma once

class NonCopyable {
protected:
    NonCopyable() = default;
    ~NonCopyable() = default;

    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};
