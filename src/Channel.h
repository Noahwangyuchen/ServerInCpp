#pragma once

#include <sys/epoll.h>
#include <functional>

class EventLoop;

/*
    Channel类用于管理epoll中的事件，包括fd、事件类型、事件状态等信息。
    Channel会留一个指针在epoll_event.data中，接受到事件后会通过这个指针找到对应的Channel对象并处理事件。
*/
class Channel {

private:
    EventLoop* loop;
    int fd;
    uint32_t events;        // 希望监听的事件
    uint32_t revents;       // 捕获到的事件
    bool in_epoll;
    std::function<void()> callback;

public:
    Channel(EventLoop* _loop, int _fd);
    ~Channel();

    void enableReading();
    void handleEvent();

    int getfd();
    uint32_t getEvents();
    uint32_t getRevents();
    bool inEpoll();
    void setInEpoll();
    void setRevents(uint32_t _revents);
    void setCallback(std::function<void()> _callback);
};