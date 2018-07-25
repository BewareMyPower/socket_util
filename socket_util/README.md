# 重构说明
## 层次说明
为了方便阅读源码，把`*.h`和`*.cc`文件均放在同一目录，并拟定命名空间层次如下
```
socket_util
  \--inet
  \--error
  \--util
```
其中util和error单独放在一个目录[helpers](./helpers)中，因为均为header-only库，直接包含即可。
目录层次
```
socket_util
  \--helpers
examples
```
examples目录存放示例程序，以及平时的测试程序
## 重构原因
暂时仅简单包装了套接字函数，因此保存之前的源码目录[include](../include)和[lib](../lib)
之前的设计是用一个Socket类持有一个文件描述符(fd)，然后封装C API为成员函数，并提供`get()`函数传递fd给C API(若必要)。
这有个问题就是本质上Socket类的成员只有一个int，其余全部都只是成员函数，只是编程风格上的差异，而且在库未最终完成之前的测试阶段，必须得大量调用未经包装的C API，这样会非常麻烦。
另一方面，所有包裹函数中都和错误处理十分耦合，不返回错误码，而是直接退出。对于学习而言，这是很简单暴力的行为，APUE等书籍都是这么做的，但是如果是编写库，错误处理应该留给用户去做。
最近学习golang，学习了其中的做法(虽然Go的错误处理方式很有争议)，把错误处理函数单独分开，保留错误码，如果需要得到返回值，会返回`std::pair<T, bool>`，虽然不如Go在语法上那样方便。
## 错误处理函数
错误处理函数的命名问题也是我花了很长时间才确定下来的，看过这几本书的做法
- Unix环境高级编程(APUE): `err_sys`/`err_ret`/`err_quit`等
- Linux/Unix系统编程手册(TLPI): `errExit`/`usageError`等
- 深入理解计算机系统(CSAPP): `unix_error`/`posix_error`/`gai_error`等

APUE的错误处理函数太多，不方便记忆，到底哪个用了`errno`哪个没用，哪个退出了哪个没退出，还有自定义`error`的，以及不采用`exit()`采用`abort()`的。
CSAPP则简单许多，全都是打印错误并退出，并且不支持格式化参数，对于书上的示例代码足够，但是功能不够强大。
TLPI则是我比较欣赏的，但仍然也有错误处理函数较多的问题。
这里我采用的方法是，打印即`Print`和`Println`，打印后退出是`Exit`，若满足条件则打印并退出是`ExitIf`，至于是否使用错误码则显式地放在第一个参数，从而使重载函数能区分。
其中`ExitIf`调用`Exit`，`Exit`调用`Println`，`Print`则较为独立，考虑是否删除。
上述函数均位于命名空间`socket_util::util::error`下，参考[errors.hpp](util/errors.hpp)
