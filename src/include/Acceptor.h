#pragma once

#include <functional>
#include <memory>
#include "Macros.h"

class EventLoop;
class Socket;
class InetAddress;
class Channel;


/*
    代表一个连接建立器。

    生命周期：Server启动时创建，Server退出时销毁。

    连接建立流程：创建时创建sock并listen,建立Channel -> Channel监听到连接,callback Acceptor创建新连接并加入loop
    -> callback Server以注册新Connection.
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