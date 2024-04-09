#pragma once

class EventLoop;
class Socket;

class Server {

private:
    EventLoop* loop;

public:
    Server(EventLoop* _loop);
    ~Server();

    void handleReadEvent(int sockfd);
    void newConnection(Socket* srv_sock);

};