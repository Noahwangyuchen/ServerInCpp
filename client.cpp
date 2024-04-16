#include "Connection.h"
#include "Socket.h"
#include <iostream>


int main() {

    Socket* sock = new Socket();
    sock->create();
    sock->connect("127.0.0.1", 8888);

    Connection* conn = new Connection(nullptr, sock->getSockfd());
    while (true) {
        conn->getlineSendBuffer();
        if (conn->getState() == Connection::State::Closed) {
            conn->close();
            break;
        }
        conn->write();
        conn->read();
        std::cout << "Message from server: " << conn->readBuffer() << std::endl;
    }

    delete conn;
    return 0;
}