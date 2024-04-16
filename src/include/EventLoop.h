#pragma once

#include <functional>
#include <Macros.h>
#include <memory>

class Epoll;
class Channel;

class EventLoop {

private:
    std::unique_ptr<Epoll> m_epoll;

public:
    EventLoop();
    ~EventLoop();

    DISALLOW_COPY_AND_MOVE(EventLoop);

    void loop();
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
};