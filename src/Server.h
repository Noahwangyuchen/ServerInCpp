#pragma once

#include <map>

class EventLoop;
class Socket;
class Connection;
class Acceptor;

class Server {

private:
    EventLoop* loop;
    Acceptor* acceptor;
    
    std::map<int, Connection*> connections;

public:
    Server(EventLoop* _loop);
    ~Server();

    void newConnection(Socket* sock);
    void deleteConnection(Socket* sock);

};