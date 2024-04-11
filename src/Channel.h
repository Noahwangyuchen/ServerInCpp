#pragma once

#include <sys/epoll.h>
#include <functional>

class EventLoop;

/*
    Channel�����ڹ���epoll�е��¼�������fd���¼����͡��¼�״̬����Ϣ��
    Channel����һ��ָ����epoll_event.data�У����ܵ��¼����ͨ�����ָ���ҵ���Ӧ��Channel���󲢴����¼���
*/
class Channel {

private:
    EventLoop* loop;
    int fd;
    uint32_t events;        // ϣ���������¼�
    uint32_t revents;       // ���񵽵��¼�
    bool in_epoll;
    bool useThreadPool;
    std::function<void()> readCallback;
    std::function<void()> writeCallback;

public:
    Channel(EventLoop* _loop, int _fd);
    ~Channel();

    void enableReading();
    void handleEvent();

    int getfd();
    uint32_t getEvents();
    uint32_t getRevents();
    bool inEpoll();
    void useET();

    void setInEpoll();
    void setRevents(uint32_t _revents);
    void setReadCallback(std::function<void()> _callback);
    void setUseThreadPool(bool use = true);
};