#include "Acceptor.h"
#include "Channel.h"
#include "Socket.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Server.h"
#include <memory>

Acceptor::Acceptor(EventLoop* _loop) : loop(_loop) {
    sock = new Socket();
    std::unique_ptr<InetAddress> addr(new InetAddress("127.0.0.1", 8888));
    sock->bind(addr.get());
    sock->listen();
    channel = new Channel(loop, sock->getSockfd());
    channel->setReadCallback(std::bind(&Acceptor::acceptConnection, this));
    channel->enableReading();
}

Acceptor::~Acceptor() {
    delete sock;
    delete channel;
}

void Acceptor::acceptConnection() {
    std::unique_ptr<InetAddress> cln_addr(new InetAddress("127.0.0.1", 8888));
    Socket* cln_sock = new Socket(sock->accept(cln_addr.get()));
    cln_sock->setnonblocking();
    printf("New client fd %d! Ip: %s, Port: %d\n", cln_sock->getSockfd(), inet_ntoa(cln_addr->addr.sin_addr), ntohs(cln_addr->addr.sin_port));
    
    newConnectionCallback(cln_sock);
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket*)> _callback) {
    newConnectionCallback = _callback;
}