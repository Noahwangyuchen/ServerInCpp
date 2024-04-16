#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>
#include "Macros.h"

class EventLoop;
class Socket;
class Connection;
class ThreadPool;
class Acceptor;

class Server {

private:
    std::unique_ptr<EventLoop> m_mainReactor;
    std::unique_ptr<Acceptor> m_acceptor;
    
    std::unordered_map<int, std::unique_ptr<Connection>> m_connections;
    std::vector<std::unique_ptr<EventLoop>> m_subReactors;
    std::unique_ptr<ThreadPool> m_pool;

    // 为用户提供的自定义业务的回调函数 
    std::function<void(Connection*)> m_onConnectionCallback;
    std::function<void(Connection*)> m_onMessageCallback;
    std::function<void(Connection*)> m_newConnectionCallback;

public:
    Server();
    void start();
    ~Server();

    DISALLOW_COPY_AND_MOVE(Server);

    RC newConnection(int fd);
    RC deleteConnection(int sockfd);

    void onConnect(std::function<void(Connection*)> const &func);
    void onMessage(std::function<void(Connection*)> const &func);
    void onNewConnection(std::function<void(Connection*)> const &func);
};