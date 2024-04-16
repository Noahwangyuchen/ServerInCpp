#include "Server.h"
#include "Connection.h"
#include "Socket.h"
#include <iostream>
#include <string>
#include <memory>
int main()
{
    std::unique_ptr<Server> server = std::make_unique<Server>();

    server->onNewConnection([](Connection* conn) {
        std::cout << "new connection from fd: " << conn->getSocket()->getSockfd() << std::endl;
    });

    server->onMessage([](Connection* conn) {
        if (conn && conn->getState() == Connection::State::Connected) {
            std::string message = conn->readBuffer();
            std::cout << "receive message from client: " << message << std::endl;
            conn->send(message);
        }
    });

    server->start();

    return 0;
}

