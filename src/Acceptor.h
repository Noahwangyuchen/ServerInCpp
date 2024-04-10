#pragma once

#include <functional>

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