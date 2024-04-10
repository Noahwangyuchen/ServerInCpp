#pragma once

#include <sys/epoll.h>
#include <vector>

class Channel;

/*
    封装了epoll,用于注册Channel和生成epoll_wait的返回值
*/
class Epoll {

private:
    int epoll_fd;
    epoll_event *events;

public:
    Epoll();
    ~Epoll();

    void updateChannel(Channel* channel);
    std::vector<Channel*> poll(int timeout = -1);

};