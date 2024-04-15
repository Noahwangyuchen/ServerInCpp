#pragma once

#include <functional>
#include "Macros.h"

class EventLoop;
class Socket;
class InetAddress;
class Channel;


/*
    ����һ�����ӽ�������

    �������ڣ�Server����ʱ������Server�˳�ʱ���١�

    ���ӽ������̣�����ʱ����sock��listen,����Channel -> Channel����������,callback Acceptor���������Ӳ�����loop
    -> callback Server��ע����Connection.
*/
class Acceptor {

private:
    EventLoop* loop;
    Socket* sock;
    Channel* channel;
    std::function<void(Socket*)> newConnectionCallback;

public:
    Acceptor(EventLoop* _loop);
    ~Acceptor();

    DISALLOW_COPY_AND_MOVE(Acceptor);

    void acceptConnection();
    void setNewConnectionCallback(std::function<void(Socket*)> const &_callback);
};