#include <iostream>
#include "Connection.h"
#include "Socket.h"
#include "ThreadPool.h"
#include <string>

void oneClient() {
    int msgs = 100, wait = 0;
    Socket* sock = new Socket();
    sock->create();
    sock->connect("127.0.0.1", 8888);

    Connection* conn = new Connection(nullptr, sock->getSockfd());
    while (msgs--) {
        std::string s = "Hello, server!";
        if (conn->getState() == Connection::State::Connected) {
            conn->send(s);
        }
        if (conn->getState() == Connection::State::Closed) {
            conn->close();
            break;
        }
        conn->read();
        std::cout <<"cnt: " << msgs << "Message from server: " << conn->readBuffer() << std::endl;
    }

    delete conn;
}

int main() {
    int threads = 10000;
    ThreadPool *pool = new ThreadPool(threads);
    std::function<void()> func = std::bind(oneClient);
    while(threads--) {
        pool->add(func);
    }
    delete pool;
    return 0;
}