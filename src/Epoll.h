#pragma once

#include <sys/epoll.h>
#include <vector>

class Channel;

class Epoll {

private:
    int epoll_fd;
    epoll_event *events;

public:
    Epoll();
    ~Epoll();

    void add_fd(int fd, uint32_t op);
    void updateChannel(Channel* channel);
    std::vector<Channel*> poll(int timeout = -1);

};