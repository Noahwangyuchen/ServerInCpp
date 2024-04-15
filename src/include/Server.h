#pragma once

#include <map>
#include <vector>
#include <functional>
#include "Macros.h"

class EventLoop;
class Socket;
class Connection;
class ThreadPool;
class Acceptor;

class Server {

private:
    EventLoop* m_mainReactor;
    Acceptor* m_acceptor;
    
    std::map<int, Connection*> m_connections;
    std::vector<EventLoop*> m_subReactors;
    ThreadPool* m_pool;

    // 为用户提供的自定义业务的回调函数 
    std::function<void(Connection*)> m_onConnectionCallback;
    std::function<void(Connection*)> m_onMessageCallback;
    std::function<void(Connection*)> m_newConnectionCallback;

public:
    Server(EventLoop* _loop);
    ~Server();

    DISALLOW_COPY_AND_MOVE(Server);

    void newConnection(Socket* sock);
    void deleteConnection(int sockfd);
    void onConnect(std::function<void(Connection*)> const &func);
    void onMessage(std::function<void(Connection*)> const &func);
    void onNewConnection(std::function<void(Connection*)> const &func);
};