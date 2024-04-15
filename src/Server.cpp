#include "Server.h"

#include <unistd.h>

#include <cstdio>
#include <cstring>
#include <memory>

#include "Acceptor.h"
#include "Connection.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"
#include "ThreadPool.h"
#include "util.h"

Server::Server(EventLoop* _loop) : m_mainReactor(_loop) {
    m_acceptor = new Acceptor(m_mainReactor);
    m_acceptor->setNewConnectionCallback(
        std::bind(&Server::newConnection, this, std::placeholders::_1));

    int size = static_cast<int>(std::thread::hardware_concurrency());
    m_pool = new ThreadPool(size);

    m_subReactors.reserve(size);
    while (size--) {
        m_subReactors.emplace_back(new EventLoop());
        m_pool->add(std::move(
            std::bind(&EventLoop::loop,
                      m_subReactors.back())));  // 每个线程都启动一个EventLoop
    }
}

Server::~Server() {
    delete m_acceptor;
    delete m_pool;
}

void Server::newConnection(Socket* sock) {
    int rand = sock->getSockfd() % m_subReactors.size();
    Connection* conn = new Connection(m_subReactors[rand], sock);
    conn->setDeleteConnectionCallback(
        std::bind(&Server::deleteConnection, this, std::placeholders::_1));
    // conn->setOnConnectCallback(m_onConnectionCallback);
    conn->setOnMessageCallback(m_onMessageCallback);
    m_connections[sock->getSockfd()] = conn;
    if (m_newConnectionCallback) {
        m_newConnectionCallback(conn);
    }
}

void Server::deleteConnection(int sockfd) {
    if (m_connections.find(sockfd) != m_connections.end()) {
        Connection* conn = m_connections[sockfd];
        delete conn;
        m_connections.erase(sockfd);
    }
}

void Server::onConnect(std::function<void(Connection*)> const &func) { 
    m_onConnectionCallback = std::move(func);
}

void Server::onMessage(std::function<void(Connection*)> const &func) {
    m_onMessageCallback = std::move(func);
}

void Server::onNewConnection(std::function<void(Connection*)> const &func) {
    m_newConnectionCallback = std::move(func);
}


