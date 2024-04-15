#pragma once

#include <sys/epoll.h>
#include <functional>
#include "Macros.h"

class EventLoop;

/*
    Channel类用于管理epoll中的事件，包括fd、事件类型、事件状态等信息。
    Channel会留一个指针在epoll_event.data中，接受到事件后会通过这个指针找到对应的Channel对象并处理事件。
*/
class Channel {

private:
    EventLoop* loop;
    int fd;
    int events;        // 希望监听的事件
    int revents;       // 捕获到的事件
    bool in_epoll;
    std::function<void()> readCallback;
    std::function<void()> writeCallback;

public:
    static const int READ_EVENT;
    static const int WRITE_EVENT;
    static const int ET;

    Channel(EventLoop* _loop, int _fd);
    ~Channel();

    DISALLOW_COPY_AND_MOVE(Channel);

    void enableRead();
    void enableWrite();
    void handleEvent();

    int getfd();
    uint32_t getEvents();
    uint32_t getRevents();
    bool inEpoll();
    void useET();

    void setInEpoll(bool _in_epoll = true);
    void setRevents(int _revents);
    void setReadCallback(std::function<void()> const &_callback);
};