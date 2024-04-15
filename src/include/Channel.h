#pragma once

#include <sys/epoll.h>
#include <functional>
#include "Macros.h"

class EventLoop;

/*
    Channel�����ڹ���epoll�е��¼�������fd���¼����͡��¼�״̬����Ϣ��
    Channel����һ��ָ����epoll_event.data�У����ܵ��¼����ͨ�����ָ���ҵ���Ӧ��Channel���󲢴����¼���
*/
class Channel {

private:
    EventLoop* loop;
    int fd;
    int events;        // ϣ���������¼�
    int revents;       // ���񵽵��¼�
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