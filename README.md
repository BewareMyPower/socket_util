# `socket_util`
对linux的socket库函数的C++封装

# 使用方式(新版本)
简单为主，不用复杂的目录结构，库放在[socket_util](socket_util)目录下，示例放在[examples](examples)目录下。
直接在目录下`make`即可，生成的是静态库。

# 使用方式(旧版本)
库的组成: [include](include)  [lib](lib)  [Make.defines](Make.defines)  [Make.inc](Make.inc)
上述文件的目录结构不能改变。
在自己的源文件目录下编写Makefile，需要在开头定义ROOT变量，然后包含根目录的Make.defines和Make.inc
可以参考示例[Makefile](unp_test/Makefile)
```
ROOT = ..  # 指明根目录
include $(ROOT)/Make.defines  # 包含编译器相关的通用变量

# TODO: 编写自己的Makefile语句

include $(ROOT)/Make.inc  # 库文件的依赖
```

# 编码风格
- 类和函数的命名均为大小写混合，对应文件的命名均为下划线+小写，参考谷歌C++命名约定。
- 表示作用域(类定义/函数定义/if/for/while)的大括号均为左括号在末尾，右括号在开头，如下所示
```
void foo(int i) {
    // ...
}
```
注释尽量简化，之前考虑过使用doxygen注释风格，后来觉得太过正式且占用较多时间编写重复性较大的注释(因为多次使用C++重载)，后来便以简洁的补充性注释为主

# 额外说明
头文件使用了`#pragma once`宏来防止头文件被多重包含，注意这个特性并不在C++标准中，只不过受到了大多数编译器(比如g++)的支持，严格遵守标准的代码应该使用`#ifndef`和`#define`来实现。
