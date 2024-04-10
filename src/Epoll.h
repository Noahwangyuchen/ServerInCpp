#pragma once

#include <sys/epoll.h>
#include <vector>

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

    void updateChannel(Channel* channel);
    std::vector<Channel*> poll(int timeout = -1);

};