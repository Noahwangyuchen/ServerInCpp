#include "Server.h"
#include "EventLoop.h"
#include "Buffer.h"
#include "Socket.h"
#include "Connection.h"
#include <iostream>
#include <string>

int main() {
    EventLoop* loop = new EventLoop();
    Server* server = new Server(loop);

    server->onNewConnection([](Connection* conn){
        std::cout << "new connection from fd: " << conn->getSocket()->getSockfd() << std::endl;
    });

    server->onMessage([](Connection* conn) {
        std::string message = conn->readBuffer();
        if (conn->getState() == Connection::State::Connected) {
            std::cout << "receive message from client: " << message << std::endl;
            conn->send(message);
        }
    });
    loop->loop();
    return 0;
}

