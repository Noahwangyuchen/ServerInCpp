#include "include/Acceptor.h"

#include <memory>

#include "include/Channel.h"
#include "include/EventLoop.h"
#include "include/InetAddress.h"
#include "include/Server.h"
#include "include/Socket.h"

Acceptor::Acceptor(EventLoop* _loop) : loop(_loop) {
    sock = new Socket();
    std::unique_ptr<InetAddress> addr(new InetAddress("127.0.0.1", 8880));
    sock->bind(addr.get());
    sock->listen();
    channel = new Channel(loop, sock->getSockfd());
    channel->setReadCallback(std::bind(&Acceptor::acceptConnection, this));
    channel->enableRead();
}

Acceptor::~Acceptor() {
    delete sock;
    delete channel;
}

void Acceptor::acceptConnection() {
    std::unique_ptr<InetAddress> cln_addr(new InetAddress("127.0.0.1", 8880));
    Socket* cln_sock = new Socket(sock->accept(cln_addr.get()));
    cln_sock->setNonblocking();
    if (newConnectionCallback) {
        newConnectionCallback(cln_sock);
    }
}

void Acceptor::setNewConnectionCallback(
    std::function<void(Socket*)> const& _callback) {
    newConnectionCallback = _callback;
}
