// poller.h
#pragma once
#include <stdint.h>
#include <sys/epoll.h>

#include <utility>
#include <vector>
#include "helpers/noncopyable.hpp"

namespace socket_util {

// TODO: 把struct epoll_event修改成可以设置回调函数的Channel类
// 对epoll相关函数的封装
class Poller final : noncopyable {
public:
    using Event = struct epoll_event;
    using EventList = std::vector<Event>;

    Poller() noexcept;  // 创建epoll描述符，若创建失败则报错并终止进程
    ~Poller();  // 关闭epoll描述符

    /**
     * 功能: 添加文件描述符及监听事件到监听列表中
     * 参数:
     *   fd     待监听的文件描述符
     *   events 监听事件，同struct epoll_event(参见epoll_ctl(2))的events字段
     *   opaque 对应的用户数据，同struct epoll_event的data字段的ptr字段
     * 返回值:
     *   若添加失败则返回false，errno被设置
     */
    bool add(int fd, uint32_t events, void* opaque = nullptr) noexcept;

    /**
     * 功能: 从监听列表中移除指定文件描述符
     * 参数:
     *   fd  待移除的文件描述符
     * 返回值:
     *   若移除失败则返回false，errno被设置
     */
    bool remove(int fd) noexcept;

    /**
     * 功能: 修改监听的文件描述符对应事件
     * 参数: 同Poller::add()方法的对应参数
     * 返回值:
     *   若修改失败则返回false，errno被设置
     */
    bool change(int fd, uint32_t events, void* opaque = nullptr) const noexcept;

    /**
     * 功能: 轮询监听列表得到被触发的事件列表
     * 参数:
     *   timeout_ms: 阻塞的毫秒数，默认设为-1则无限阻塞，设为0即不阻塞立刻返回
     * 返回值:
     *   若轮询成功，则返回得到的事件列表; 否则返回空列表;
     * 说明:
     *   如果timeout_ms不为默认值，即使轮询成功也可能返回空列表
     *   此时需要检查errno来判断出错原因是轮询错误还是暂无事件被触发
     */
    EventList poll(int timeout_ms = -1) const noexcept;

private:
    int epfd_;
    int num_events_ = 0;  // 监听列表数量
};

}  // END namespace socket_util
