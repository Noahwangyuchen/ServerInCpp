#include "Server.h"

#include <unistd.h>

#include <cstdio>
#include <cstring>
#include <memory>

#include "Acceptor.h"
#include "Connection.h"
#include "EventLoop.h"
#include "Socket.h"
#include "ThreadPool.h"
#include "util.h"

Server::Server() {
    m_mainReactor = std::make_unique<EventLoop>();
    m_acceptor = std::make_unique<Acceptor>(m_mainReactor.get());
    m_acceptor->setNewConnectionCallback(
        std::bind(&Server::newConnection, this, std::placeholders::_1));

    int size = static_cast<int>(std::thread::hardware_concurrency());
    m_pool = std::make_unique<ThreadPool>(size);
    m_subReactors.reserve(size);
    while (size--) {
        m_subReactors.emplace_back(std::move(std::make_unique<EventLoop>()));
    }
}

void Server::start() {
    for (auto &subReactor : m_subReactors) {
        m_pool->add(std::move(
            std::bind(&EventLoop::loop,
                      subReactor.get())));  // 每个线程都启动一个EventLoop
    }
    m_mainReactor->loop();
}

Server::~Server() {}

RC Server::newConnection(int fd) {
    int rand = fd % m_subReactors.size();
    std::unique_ptr<Connection> conn =
        std::make_unique<Connection>(m_subReactors[rand].get(), fd);
    conn->getSocket()->setNonblocking();
    conn->setDeleteConnectionCallback(
        std::bind(&Server::deleteConnection, this, std::placeholders::_1));
    // conn->setOnConnectCallback(m_onConnectionCallback);
    conn->setOnMessageCallback(m_onMessageCallback);
    m_connections[fd] = std::move(conn);
    if (m_newConnectionCallback) {
        m_newConnectionCallback(m_connections[fd].get());
    }
    return RC::RC_SUCCESS;
}

RC Server::deleteConnection(int sockfd) {
    if (m_connections.find(sockfd) != m_connections.end()) {
        m_connections.erase(sockfd);
    } else {
        perror("deleteConnection error, sockfd not found in connections");
    }
    return RC::RC_SUCCESS;
}

void Server::onConnect(std::function<void(Connection *)> const &func) {
    m_onConnectionCallback = func;
}

void Server::onMessage(std::function<void(Connection *)> const &func) {
    m_onMessageCallback = func;
}

void Server::onNewConnection(std::function<void(Connection *)> const &func) {
    m_newConnectionCallback = func;
}
