#pragma once

#include "Macros.h"

class InetAddress;

class Socket {

private:
    int sockfd;

public:
    Socket();
    Socket(int fd);
    ~Socket();

    DISALLOW_COPY_AND_MOVE(Socket);

    void bind(InetAddress* addr);
    void listen();
    void setNonblocking();
    bool isNonblocking();
    int accept(InetAddress* addr);
    void connect(InetAddress* addr);
    void connect(const char *ip, uint16_t port);
    int getSockfd();

};