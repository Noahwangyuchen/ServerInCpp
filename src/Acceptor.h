#pragma once

#include <functional>

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

    void acceptConnection();
    void setNewConnectionCallback(std::function<void(Socket*)> _callback);
};