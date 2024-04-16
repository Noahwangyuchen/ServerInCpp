#pragma once

#include <functional>
#include <memory>
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
    std::unique_ptr<Socket> m_sock;
    std::unique_ptr<Channel> m_channel;
    std::function<void(int)> m_newConnectionCallback;

public:
    explicit Acceptor(EventLoop* _loop);
    ~Acceptor();

    DISALLOW_COPY_AND_MOVE(Acceptor);

    RC acceptConnection() const;
    void setNewConnectionCallback(std::function<void(int)> const &_callback);
};