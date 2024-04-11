#pragma once

#include <functional>

class Epoll;
class Channel;
class ThreadPool;

class EventLoop {

private:
    Epoll* ep;
    ThreadPool *pool;
    bool quit;

public:
    EventLoop();
    ~EventLoop();
    void loop();
    void updateChannel(Channel* channel);
    void addToPool(std::function<void()> func);
};