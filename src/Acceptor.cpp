#include "include/Acceptor.h"

#include <memory>

#include "include/Channel.h"
#include "include/EventLoop.h"
#include "include/Server.h"
#include "include/Socket.h"

Acceptor::Acceptor(EventLoop* _loop) {
    m_sock = std::make_unique<Socket>();
    m_sock->create();
    m_sock->bind("127.0.0.1", 8888);
    m_sock->listen();
    m_channel = std::make_unique<Channel>(_loop, m_sock->getSockfd());
    m_channel->setReadCallback(std::bind(&Acceptor::acceptConnection, this));
    m_channel->enableRead();
}

Acceptor::~Acceptor() {}

RC Acceptor::acceptConnection() const {
    int cln_fd = -1;
    if (m_sock->accept(cln_fd) != RC::RC_SUCCESS) {
        return RC::RC_ACCEPTOR_ERROR;
    }
    if (m_newConnectionCallback) {
        m_newConnectionCallback(cln_fd);
    }
    return RC::RC_SUCCESS;
}

void Acceptor::setNewConnectionCallback(
    std::function<void(int)> const& _callback) {
    m_newConnectionCallback = _callback;
}
