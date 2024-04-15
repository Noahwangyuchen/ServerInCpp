#pragma once

#include <functional>
#include <Macros.h>

class Epoll;
class Channel;

class EventLoop {

private:
    Epoll* ep;
    bool quit;

public:
    EventLoop();
    ~EventLoop();

    DISALLOW_COPY_AND_MOVE(EventLoop);

    void loop();
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
};