#pragma once

#include <sys/epoll.h>
#include <vector>
#include "Macros.h"

class Channel;

/*
    ��װ��epoll,����ע��Channel������epoll_wait�ķ���ֵ
*/
class Epoll {

private:
    int epoll_fd;
    epoll_event *events;

public:
    Epoll();
    ~Epoll();

    DISALLOW_COPY_AND_MOVE(Epoll);

    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    std::vector<Channel*> poll(int timeout = -1);
};