#pragma once

#include <sys/epoll.h>
#include <vector>

class Epoll {

private:
    int epoll_fd;
    epoll_event *events;

public:
    Epoll();
    ~Epoll();

    void add_fd(int fd, uint32_t op);
    std::vector<epoll_event> poll(int timeout = -1);

};