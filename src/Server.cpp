#include "Server.h"
#include "Socket.h"
#include "EventLoop.h"
#include "Channel.h"
#include "Acceptor.h"
#include "InetAddress.h"
#include "Connection.h"
#include "util.h"
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <memory>


Server::Server(EventLoop* _loop) : loop(_loop) {
    acceptor = new Acceptor(loop);
    acceptor->setNewConnectionCallback(std::bind(&Server::newConnection, this, std::placeholders::_1));
}

Server::~Server() {
    delete acceptor;
}


void Server::newConnection(Socket* sock) {
    Connection* conn = new Connection(loop, sock);
    conn->setDeleteConnectionCallback(std::bind(&Server::deleteConnection, this, std::placeholders::_1));
    connections[sock->getSockfd()] = conn;
}

void Server::deleteConnection(Socket* sock) {
    Connection* conn = connections[sock->getSockfd()];
    delete conn;
    connections.erase(sock->getSockfd());
}
