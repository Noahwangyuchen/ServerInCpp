#pragma once

class InetAddress;

class Socket {

private:
    int sockfd;

public:
    Socket();
    Socket(int fd);
    ~Socket();

    void bind(InetAddress* addr);
    void listen();
    void setnonblocking();
    int accept(InetAddress* addr);
    void connect(InetAddress* addr);
    int getSockfd();

};