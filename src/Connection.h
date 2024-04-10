#pragma once

#include <functional>

class EventLoop;
class Socket;
class Channel;

/*
    代表一个TCP连接。
    
    生命周期：Acceptor建立连接,callback Server -> Server创建Connection ->
        Channel回调Connection -> Connection中发现断开连接 -> callback Server,注销Connection

    连接事件处理流程：EventLoop获取事件 -> Channel -> Connection
*/
class Connection {

private:
    EventLoop* loop;
    Socket* sock;
    Channel* channel;
    std::function<void(Socket*)> deleteConnectionCallback;

public:
    Connection(EventLoop* _loop, Socket* _sock);
    ~Connection();

    void echo(int sockfd);
    void setDeleteConnectionCallback(std::function<void(Socket*)> callback);
};